#include <iostream>
#include "Error.hpp"

Error::Error(enum errType t_err_no, const char* t_err_title) : _err_no(t_err_no), _err_title(t_err_title)
{
}

enum Error::errType	Error::getErrNo(void) const
{
	return _err_no;
}

void	Error::ftPerror() const
{
	std::cerr << "\033[31m[ERROR]\033[0m ";

	if (_err_title)
	{
		std::cerr << _err_title << ": ";
	}

	switch (_err_no)
	{
		case ESYSERR:
			std::perror(NULL);
			break;
		case EARGC:
			std::cerr << "Enter 2 arguments: <port> <password>" << std::endl;
			break;
		case EAPORT:
			std::cerr << "Enter a valid port number: 0~65535" << std::endl;
			break;
		case ECRLF:
			std::cerr << "Line must end with crlf" << std::endl;
			break;
		case EWRPARM:
			std::cerr << "Wrong parameters" << std::endl;
			break;
		default: // not yet
			break;
	}
}
