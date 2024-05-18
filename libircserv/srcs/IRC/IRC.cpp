#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cinttypes>
#include <iostream>
#include "IRC.hpp"
#include "Channel.hpp"
#include "FatalError.hpp"

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
			((void (*)(IRC&, int))_eventlist[i].udata)(*this, _eventlist[i].ident);
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
    // Channel 객체를 직접 생성하여 맵에 삽입
    _channels.insert(std::make_pair(channel_name, Channel(client, prefix, channel_name)));

    // insert 결과에서 삽입된 객체의 주소를 반환
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

void	IRC::acceptClient(IRC& server, int server_socket) throw(Signal, FatalError)
{
	struct kevent		event;
	struct sockaddr_in	client_addr;
	socklen_t			socklen = sizeof(struct sockaddr_in);

	std::cout << "acceptClient" << std::endl;
	// addr 초기화
	int client_fd = wrapSyscall(accept(server.getServerSocketFd(), (struct sockaddr*)&client_addr, &socklen), "accept");
	Client	client(client_fd, client_addr);

	server._clients[client.getSocketFd()] = client;

	EV_SET(&event, client.getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, (void*)receiveMessages);
	server._changelist.push_back(event);

	EV_SET(&event, client.getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void*)sendMessages);
	server._changelist.push_back(event);
}

void	IRC::receiveMessages(IRC& server, int client_socket) throw(Signal, FatalError)
{
	// test
	std::cout << "Read event" << std::endl;
	char	buf[10];
	if (read(client_socket, buf, 10) == 0)
	{
		close(client_socket);
	}
}

void	IRC::sendMessages(IRC& server, int client_socket) throw(Signal, FatalError)
{
	std::cout << "Write event" << std::endl;
}
