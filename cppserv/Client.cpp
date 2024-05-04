#include <fcntl.h>
#include "Client.hpp"

Client::Client(int _sockfd) : m_sockfd(_sockfd)
{
	fcntl(m_sockfd, F_SETFL, O_NONBLOCK);
}

int	Client::getSocketFd() const
{
	return m_sockfd;
}

int	Client::getWritable() const
{
	return m_writable;
}

void	Client::setWritable(bool _writable)
{
	m_writable = _writable;
}
