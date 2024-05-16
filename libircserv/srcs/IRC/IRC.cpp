#include <sys/socket.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "IRC.hpp"
#include "Channel.hpp"
#include "Error.hpp"

extern int	kq;

IRC::IRC(struct AccessData access_data) : _port(access_data.port), _ip_addr("127.0.0.1"), _password(access_data.password)
{
}

IRC::~IRC()
{
	close(_server_sockfd);
}

int	IRC::getServerSocketFd() const
{
	return _server_sockfd;
}

const std::string& IRC::getPassword() const
{
	return _password;
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
	std::map<int, Client>::iterator it;
	std::map<int, Client>::iterator ite;

	it = _clients.begin();
	ite = _clients.end();

	for (; it != ite; it++)
	{
		if ((it->second).getNickname() == nickname)
		{
			return &(it->second);
		}
	}

	return NULL;
}

Channel* IRC::searchChannel(const std::string& channel_name)
{
	std::map<std::string, Channel>::iterator it;

	it = _channels.find(channel_name);
	if (it == _channels.end())
		return NULL;

	return &(it->second);
}

Channel* IRC::createChannel(Client& client, const char prefix, const std::string& channel_name)
{
    // Channel 객체를 직접 생성하여 맵에 삽입
    _channels.insert(std::make_pair(channel_name, Channel(client, prefix, channel_name)));

    // insert 결과에서 삽입된 객체의 주소를 반환
	return &(_channels[channel_name]);
}

void	IRC::boot()
{
	struct sockaddr_in	server_sockaddr;
	struct kevent		event;

	setUpSocket();

	server_sockaddr = setSockAddrIn(AF_INET);
	wrapSyscall(bind(_server_sockfd, (struct sockaddr*)(&server_sockaddr), sizeof(struct sockaddr_in)), "bind");

	wrapSyscall(listen(_server_sockfd, MAX_CLIENTS), "listen");

	kq = wrapSyscall(kqueue(), "kqueue");
	EV_SET(&event, _server_sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	_changelist.push_back(event);
}

void	IRC::setUpSocket() throw(Error)
{
	_server_sockfd = wrapSyscall(socket(AF_INET, SOCK_STREAM, 0), "socket");
	
	int	opt;
	opt = 1;
	wrapSyscall(setsockopt(_server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");
}

struct sockaddr_in	IRC::setSockAddrIn(int domain) throw(Error)
{
	struct sockaddr_in	sockaddr;

	sockaddr.sin_family = domain;
	sockaddr.sin_port = htons(_port);
	sockaddr.sin_addr.s_addr = wrapSyscall(inet_addr(_ip_addr), "inet_addr");

	return sockaddr;
}
