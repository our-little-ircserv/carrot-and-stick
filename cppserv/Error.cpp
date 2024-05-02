#include <iostream>
#include "Error.hpp"

Error::Error(enum errType type, const char* title) : err_no(type), err_title(title)
{
}

enum Error::errType	Error::getErrNo(void) const
{
	return this->err_no;
}

void	Error::ft_perror() const
{
	std::cerr << "\033[31m[ERROR]\033[0m ";
	if (this->err_title)
		std::cerr << err_title << ": ";
	switch (this->err_no)
	{
		case EARGC:
			std::cerr << "Enter 2 arguments: <port> <password>" << std::endl;
			break;
		case EAPORT:
			std::cerr << "Enter a valid port number: 0~65535" << std::endl;
			break;
		default: // not yet
			break;
	}
}
