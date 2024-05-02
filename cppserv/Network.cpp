#include <sys/socket.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "Network.hpp"

Network::Network(uint16_t _port) : port(_port), ip_addr("127.0.0.1")
{
}

int	Network::getServerSocketFd() const
{
	return server_sockfd;
}

void	Network::boot()
{
	setUpSocket();

	struct sockaddr_in	server_sockaddr;
	server_sockaddr = setSockAddrIn(AF_INET);
}

void	Network::setUpSocket() throw(Error)
{
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd == -1)
		throw Error(Error::ESYSERR, "socket");
	
	int	opt;
	opt = 1;
	if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw Error(Error::ESYSERR, "setsockopt");
}

struct sockaddr_in	Network::setSockAddrIn(int domain) throw(Error)
{
	struct sockaddr_in	sockaddr;

	sockaddr.sin_family = domain;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = inet_addr(ip_addr);
	if (sockaddr.sin_addr.s_addr == (in_addr_t)-1)
		throw Error(Error::ESYSERR, "inet_addr");

	return sockaddr;
}
