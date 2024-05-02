#include "Error.hpp"
#include "Parser.hpp"
#include <limits.h>
#include <iostream>
#include <sstream>

extern std::string		password;

uint16_t	Parser::checkArgValidity(int argc, char** argv) throw(Error)
{
	if (argc != 3)
	{
		throw Error(Error::EARGC, NULL);
	}

	std::string	port(argv[1]);
	std::istringstream iss(port);
	int	port_checker;
	iss >> port_checker;
	if (iss.fail() || !iss.eof() \
			|| port_checker < 0 || port_checker > USHRT_MAX)
	{
		throw Error(Error::EAPORT, NULL);
	}

	password = argv[2];
	return (uint16_t)port_checker;
}
