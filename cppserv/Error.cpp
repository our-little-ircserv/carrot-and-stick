#include <iostream>
#include "Error.hpp"

Error::Error(enum errType _err_no, const char* _err_title) : m_err_no(_err_no), m_err_title(_err_title)
{
}

enum Error::errType	Error::getErrNo(void) const
{
	return m_err_no;
}

void	Error::ftPerror() const
{
	std::cerr << "\033[31m[ERROR]\033[0m ";

//	if (m_err_title)
//	{
//		std::cerr << m_err_title << ": ";
//	}

	switch (m_err_no)
	{
		case ESYSERR:
			std::perror(m_err_title);
			break;
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
