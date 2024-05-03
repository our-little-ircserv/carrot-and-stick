#include "Client.hpp"

Client::Client(int _sockfd) : sockfd(_sockfd)
{
}

int	Client::getSocketFd() const
{
	return sockfd;
}
