#include <cstring>
#include <sys/errno.h>
#include "FatalError.hpp"

FatalError::FatalError(enum errType t_err_no, const char* t_err_title) : Error(static_cast< unsigned int >(t_err_no), t_err_title)
{
}

FatalError::~FatalError()
{
}

std::string	FatalError::what() const
{
	std::string	error_message;

	if (_err_title != NULL)
	{
		error_message = _err_title + ": ";
	}

	switch (_err_no)
	{
		case ESYSERR:
			error_message += std::strerror(errno);
			break;
		case EARGC:
			error_message += "Enter 2 arguments: <port> <password>";
			break;
		case EAPORT:
			error_message += "Enter a valid port number: 0~65535";
	}

	return error_message;
}
