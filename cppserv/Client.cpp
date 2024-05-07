#include <fcntl.h>
#include "Client.hpp"

Client::Client(int t_sockfd) : _sockfd(t_sockfd)
{
	fcntl(_sockfd, F_SETFL, O_NONBLOCK);
}

int	Client::getSocketFd() const
{
	return _sockfd;
}

int	Client::getWritable() const
{
	return _writable;
}

void	Client::setWritable(bool t_writable)
{
	_writable = t_writable;
}
