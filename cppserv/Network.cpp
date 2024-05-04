#include <sys/socket.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "Network.hpp"
#include "Error.hpp"

Network::Network(uint16_t _port) : m_port(_port), m_ip_addr("127.0.0.1")
{
}

Network::~Network()
{
	close(m_server_sockfd);
}

int	Network::getServerSocketFd() const
{
	return m_server_sockfd;
}

void	Network::boot()
{
	setUpSocket();

	struct sockaddr_in	server_sockaddr;
	server_sockaddr = setSockAddrIn(AF_INET);
	wrapSyscall(bind(m_server_sockfd, (struct sockaddr*)(&server_sockaddr), sizeof(struct sockaddr_in)), "bind");

	wrapSyscall(listen(m_server_sockfd, MAX_CLIENTS), "listen");

	std::cout << "\033[32m[SERVER STARTED]\033[0m" << std::endl;
}

void	Network::setUpSocket() throw(Error)
{
	m_server_sockfd = wrapSyscall(socket(AF_INET, SOCK_STREAM, 0), "socket");
	
	int	opt;
	opt = 1;
	wrapSyscall(setsockopt(m_server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");
}

struct sockaddr_in	Network::setSockAddrIn(int domain) throw(Error)
{
	struct sockaddr_in	sockaddr;

	sockaddr.sin_family = domain;
	sockaddr.sin_port = htons(m_port);
	sockaddr.sin_addr.s_addr = wrapSyscall(inet_addr(m_ip_addr), "inet_addr");

	return sockaddr;
}
