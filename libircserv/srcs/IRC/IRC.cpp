#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cinttypes>
#include <iostream>
#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "FatalError.hpp"
#include "Assert.hpp"

extern int	kq;

const char*	IRC::hostname = "carrot-and-stick.ircserv.com";

IRC::IRC(struct AccessData access_data) : _port(access_data.port), _ip_addr("127.0.0.1"), _password(access_data.password)
{
}

IRC::~IRC()
{
	close(_server_sockfd);
}

void	IRC::init() throw(Signal, FatalError)
{
	struct sockaddr_in	server_sockaddr;
	struct kevent		event;

	setUpSocket();

	server_sockaddr = setSockAddrIn(AF_INET);
	wrapSyscall(bind(_server_sockfd, (struct sockaddr*)(&server_sockaddr), sizeof(struct sockaddr_in)), "bind");

	wrapSyscall(listen(_server_sockfd, MAX_CLIENTS), "listen");

	kq = wrapSyscall(kqueue(), "kqueue");
	EV_SET(&event, _server_sockfd, EVFILT_READ, EV_ADD, 0, 0, (void*)acceptClient);
	_changelist.push_back(event);

	Command::init();
}

void	IRC::run() throw(Signal, FatalError)
{
	struct timespec		time;
	time.tv_sec = 3;
	time.tv_nsec = 0;

	while (true)
	{
		int	real_events = wrapSyscall(kevent(kq, &(_changelist.front()), _changelist.size(), _eventlist, MAX_EVENTS, &time), "kevent");
		_changelist.clear();

		for (int i = 0; i < real_events; i++)
		{
			reinterpret_cast< void (*)(IRC&, struct kevent&) >(_eventlist[i].udata)(*this, _eventlist[i]);
		}
	}
}

Channel* IRC::searchChannel(const std::string& channel_name)
{
	std::map<std::string, Channel>::iterator it;

	it = _channels.find(channel_name);
	if (it == _channels.end())
	{
		return NULL;
	}

	return &(it->second);
}

Channel* IRC::createChannel(Client& client, const char prefix, const std::string& channel_name)
{
    _channels.insert(std::make_pair(channel_name, Channel(client, prefix, channel_name)));

	return &(_channels[channel_name]);
}

void	IRC::setUpSocket() throw(Signal, FatalError)
{
	_server_sockfd = wrapSyscall(socket(AF_INET, SOCK_STREAM, 0), "socket");
	fcntl(_server_sockfd, F_SETFL, O_NONBLOCK);
	
	int	opt;
	opt = 1;
	wrapSyscall(setsockopt(_server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");
}

Client* IRC::searchClient(const int sockfd)
{
	if (_clients.find(sockfd) == _clients.end())
	{
		return NULL;
	}

	return &(_clients[sockfd]);
}

Client* IRC::searchClient(const std::string& nickname)
{
	std::map<int, Client>::iterator it = _clients.begin();
	std::map<int, Client>::iterator ite = _clients.end();

	for (; it != ite; it++)
	{
		if ((it->second).getNickname() == nickname)
		{
			return &(it->second);
		}
	}

	return NULL;
}

void	IRC::delClient(const int sockfd)
{
	close(sockfd);
	_clients.erase(_clients.find(sockfd));
}

int	IRC::getServerSocketFd() const
{
	return _server_sockfd;
}

const std::string& IRC::getPassword() const
{
	return _password;
}

struct sockaddr_in	IRC::setSockAddrIn(int domain) throw(Signal, FatalError)
{
	struct sockaddr_in	sockaddr;

	sockaddr.sin_family = domain;
	sockaddr.sin_port = htons(_port);
	sockaddr.sin_addr.s_addr = wrapSyscall(inet_addr(_ip_addr), "inet_addr");

	return sockaddr;
}

void	IRC::get_next_line(Client& client, const std::string& input)
{
	std::vector< std::string >&	rdbuf = client._read_buf;
	std::string&				last_element = rdbuf.back();
	bool						add_than_push = false;

	if (rdbuf.empty() == false && last_element.substr(last_element.size() - 2, 2) != "\r\n")
	{
		add_than_push = true;
	}

	size_t	old_idx = 0;
	size_t	crlf = 0;
	size_t	input_size = input.size();
	size_t	substr_size;
	while (old_idx < input_size)
	{
		crlf = input.find("\r\n", old_idx);
		if (crlf != std::string::npos)
		{
			substr_size = crlf - old_idx + 2;
		}
		else
		{
			substr_size = input_size - old_idx;
		}

		if (add_than_push == true)
		{
			last_element += input.substr(old_idx, substr_size);
			add_than_push = false;
		}
		else
		{
			rdbuf.push_back(input.substr(old_idx, substr_size));
		}

		old_idx += substr_size;
	}
}

void	IRC::empty_inputs(IRC& server, Client& client)
{
	std::vector< std::string >::iterator	it = client._read_buf.begin();

	for (; it != client._read_buf.end(); )
	{
		if (it->empty() == false && it->substr(it->size() - 2, 2) == "\r\n")
		{
			std::cout << *it << std::endl;
			struct Parser::Data data = Parser::parseClientMessage(*it);
			Command::execute(server, client, data);
			it = client._read_buf.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void	IRC::acceptClient(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	struct kevent		t_event;
	struct sockaddr_in	client_addr;
	socklen_t			socklen = sizeof(struct sockaddr_in);

	std::cout << "acceptClient" << std::endl;
	int client_fd = wrapSyscall(accept(server.getServerSocketFd(), (struct sockaddr*)&client_addr, &socklen), "accept");
	Client	client(client_fd, client_addr);

	server._clients[client.getSocketFd()] = client;

	EV_SET(&t_event, client.getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, (void*)receiveMessages);
	server._changelist.push_back(t_event);

	EV_SET(&t_event, client.getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void*)sendMessages);
	server._changelist.push_back(event);
}

void	IRC::receiveMessages(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	std::cout << "Read event" << std::endl;

	Client*	client = server.searchClient((int)event.ident);
	Assert(client != NULL);

	if (event.data == 0)
	{
		server.delClient(event.ident);
		return;
	}

	char	buf[512];
//	char*	buf = new char[event.data + 1];
//	Assert(buf != NULL);

	int	bytes_received = recv(event.ident, buf, event.data, 0);
	if (bytes_received != -1)
	{
		buf[bytes_received] = '\0';
		get_next_line(*client, buf);
	}
//	delete[] buf;
	wrapSyscall(bytes_received, "recv");

	empty_inputs(server, *client);
}

// kevent EVFILT_WRITE를 감지한 경우 - event는 실시간
// write_buf에 쌓인 걸 하나만 내보낼지
// 아니면 한번에 다 내보낼지
void	IRC::sendMessages(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	std::cout << "Write event" << std::endl;

	Client*	client = server.searchClient((int)event.ident);
	Assert(client != NULL);

	std::vector< std::string >::iterator it = client->_write_buf.begin();
	std::vector< std::string >::iterator ite = client->_write_buf.end();
	for ( ; it != ite; it++)
	{
		wrapSyscall(send(event.ident, it->c_str(), it->size(), 0), "send");
	}
	client->_write_buf.clear();

//	wrapSyscall(send(event.ident, client->_write_buf.first().c_str(), it->size(), 0), "send");
//	client->_write_buf.erase(client->_write_buf.begin());
}
