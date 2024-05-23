#include <sys/socket.h>
#include <sys/stat.h>
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

std::vector< Channel* >	IRC::delClient(Client& client)
{
	std::vector< Channel* >		empty_channels;
	std::vector< std::string >	chan_list;
	chan_list.insert(chan_list.end(), client.getChannelList().begin(), client.getChannelList().end());
	for (size_t i = 0; i < chan_list.size(); i++)
	{
		Channel* channel = searchChannel(chan_list[i]);
		Assert(channel != NULL && channel->isMember(client) == true);

		if (channel->isOperator(client) == true)
		{
			channel->delOperator(client);
		}
		channel->delMember(client);

		if (channel->isEmpty() == true)
		{
			empty_channels.push_back(channel);
		}
	}

	close(client.getSocketFd());
	_clients.erase(_clients.find(client.getSocketFd()));

	return empty_channels;
}

void	IRC::delChannels(const std::vector< Channel* >& channels)
{
	size_t	channels_size = channels.size();
	for (size_t i = 0; i < channels_size; i++)
	{
		_channels.erase(_channels.find(channels[i]->getChannelName()));
	}
}

int	IRC::getServerSocketFd() const
{
	return _server_sockfd;
}

const std::string& IRC::getPassword() const
{
	return _password;
}

void	IRC::deliverMsg(std::set< Client* >& target_list, std::string msg)
{
	std::set< Client* >::iterator it = target_list.begin();
	std::set< Client* >::iterator ite = target_list.end();

	for (; it != ite; it++)
	{
		(*it)->_write_buf.push_back(msg);
		if ((*it)->_writable == false)
		{
			(*it)->_writable = true;
			struct kevent	t_event;
			EV_SET(&t_event, (*it)->getSocketFd(), EVFILT_WRITE, EV_ENABLE, 0, 0, (void*)sendMessages);
			_changelist.push_back(t_event);
		}
	}
}

std::set< Client* > IRC::getTargetSet(std::vector< std::string >targets)
{
	std::set< Client* > ret;
	std::set< Client* > t_list;

	for (size_t i = 0; i < targets.size(); i++)
	{
		Channel* channel = searchChannel(targets[i]);
		if (channel != NULL)
		{
			t_list = channel->getMemberSet();
			ret.insert(t_list.begin(), t_list.end());
			continue ;
		}

		Client* client = searchClient(targets[i]);
		if (client != NULL)
		{
			ret.insert(client);
		}
	}

	return ret;
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
	bool						add_than_push = false;
	size_t						old_idx = 0;
	size_t						crlf = 0;

	std::string&	last_element = rdbuf.back();
	if (client._read_buf.empty() == false && last_element.find("\r\n") == std::string::npos)
	{
		crlf = input.find("\r\n");
		old_idx = crlf;
		if (crlf != std::string::npos)
		{
			old_idx += 2;
		}

		last_element += input.substr(0, old_idx);
	}

	size_t	input_size = input.size();
	size_t	substr_size;
	while (old_idx < input_size)
	{
		crlf = input.find("\r\n", old_idx);
		substr_size = crlf;
		if (crlf == std::string::npos)
		{
			substr_size = input_size;
		}
		else
		{
			substr_size = crlf + 2;
		}
		substr_size -= old_idx;

		rdbuf.push_back(input.substr(old_idx, substr_size));

		old_idx += substr_size;
	}
}

void	IRC::iterate_rdbuf(IRC& server, Client& client)
{
	std::vector< std::string >::iterator	it = client._read_buf.begin();

	for ( ; it != client._read_buf.end(); )
	{
		if (it->empty() == false && it->substr(it->size() - 2, 2) == "\r\n")
		{
			struct Parser::Data data = Parser::parseClientMessage(*it);
			Command::execute(server, client, data);
			if (client.getRegisterLevel() == Client::LEFT)
			{
				break;
			}
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

	if ((event.flags & EV_EOF) != 0)
	{
		server.disconnectClient(*client);
		return;
	}

	char	buf[512];

	int	bytes_received = wrapSyscall(recv(event.ident, buf, event.data, 0), "recv");
	buf[bytes_received] = '\0';

	get_next_line(*client, buf);
	iterate_rdbuf(server, *client);

	if (client->getRegisterLevel() == Client::LEFT)
	{
		server.disconnectClient(*client);
		std::cout << "closed successfully" << std::endl;
	}
}

void	IRC::sendMessages(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	std::cout << "Write event" << std::endl;

	struct stat	statbuf;
	if (fstat(event.ident, &statbuf) == -1)
	{
		return;
	}

	Client*	client = server.searchClient((int)event.ident);
	Assert(client != NULL);

	// 한번에 보낼 수 있는 최대 블럭수
	size_t	sent_size;
	while (client->_write_buf.empty() == false)
	{
		size_t	data_size = client->_write_buf[0].size();
		sent_size = wrapSyscall(send(event.ident, client->_write_buf[0].c_str(), data_size, 0), "send");
		if (sent_size < data_size)
		{
			client->_write_buf[0] = client->_write_buf[0].substr(sent_size);
			break;
		}
		client->_write_buf.erase(client->_write_buf.begin());
	}

	if (client->_write_buf.empty() == true && client->_writable == true)
	{
		client->_writable = false;

		struct kevent	t_event;
		EV_SET(&t_event, client->getSocketFd(), EVFILT_WRITE, EV_DISABLE, 0, 0, (void*)sendMessages);
		server._changelist.push_back(t_event);
	}
}
