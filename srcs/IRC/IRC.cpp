#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cinttypes>
#include <sstream>
#include "IRC.hpp"
#include "IEventHandler.hpp"
#include "Parser.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "FatalError.hpp"
#include "Assert.hpp"

extern int	kq;

const std::string	IRC::hostname = "carrot-and-stick.ircserv.com";

IRC::IRC(struct AccessData access_data) : _port(access_data.port), _ip_addr("127.0.0.1"), _password(access_data.password)
{
	std::time_t	now = std::time(0);
	_start_time = std::localtime(&now);
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
	EV_SET(&event, _server_sockfd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast< void* >(&_server_event_handler));
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
			IEventHandler*	handler = static_cast< IEventHandler* >(_eventlist[i].udata);
			if (handler == NULL)
			{
				continue;
			}

			if (_eventlist[i].filter == EVFILT_READ)
			{
				handler->read(*this, _eventlist[i]);
			}
			else if (_eventlist[i].filter == EVFILT_WRITE)
			{
				handler->write(*this, _eventlist[i]);
			}
		}
	}
}

void	IRC::pushEvent(struct kevent& event)
{
	_changelist.push_back(event);
}

Channel* IRC::searchChannel(const std::string& channel_name)
{
	std::map< std::string, Channel >::iterator it;

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
	std::map< int, Client >::iterator it;

	it = _clients.find(sockfd);
	if (it == _clients.end())
	{
		return NULL;
	}

	return &(it->second);
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

Client&	IRC::createClient(int sockfd, struct sockaddr_in addr)
{
	_clients.insert(std::make_pair(sockfd, Client(sockfd, addr)));

	return _clients[sockfd];
}

// client socket disconnect
void	IRC::disconnectClient(Client& client)
{
	int	t_sockfd = client.getSocketFd();
	std::vector< Channel* >	empty_channels = delClient(client);
	delChannels(empty_channels);
	close(t_sockfd);
}

// map에 담긴 client 객체 소멸
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

		if (channel->getMemberCnt() == 0)
		{
			empty_channels.push_back(channel);
		}
	}

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

std::string	IRC::getStartTime() const
{
	std::stringstream	ss;

	ss << (_start_time->tm_year + 1900) << "-" << (_start_time->tm_mon + 1) << "-" << _start_time->tm_mday;

	return ss.str();
}

int	IRC::getServerSocketFd() const
{
	return _server_sockfd;
}

const std::string& IRC::getPassword() const
{
	return _password;
}

ServerEventHandler&	IRC::getServerEventHandler()
{
	return _server_event_handler;
}

ClientEventHandler&	IRC::getClientEventHandler()
{
	return _client_event_handler;
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
			EV_SET(&t_event, (*it)->getSocketFd(), EVFILT_WRITE, EV_ENABLE, 0, 0, reinterpret_cast< void* >(&_client_event_handler));
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
	size_t						old_idx = 0;
	size_t						crlf = 0;

	std::string&	last_element = rdbuf.back();
	if (rdbuf.empty() == false && last_element.find("\r\n") == std::string::npos)
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
		Assert(it->empty() == false);
		size_t	content_size = it->size();
		if (it->find("\r\n") != std::string::npos)
		{
			if (content_size > 2 && content_size <= 512)
			{
				struct Parser::Data data = Parser::parseClientMessage(*it);
				Command::execute(server, client, data);
			}

			it = client._read_buf.erase(it);
		}
		else
		{
			break;
		}
	}
}
