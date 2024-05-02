#include <signal.h>
#include <iostream>
#include "Network.hpp"
#include "Error.hpp"
#include "Parser.hpp"

std::string		password;

int	main(int argc, char** argv)
{
	int	err_no = 0;
	signal(SIGINT, SIG_IGN);

	try
	{
		Network	network(Parser::checkArgValidity(argc, argv));
		network.boot();
		for(;;);
	}
	catch (Error& err)
	{
		err.ft_perror();
		err_no = err.getErrNo();

		goto TERMINATE_PROGRAM;
	}

	goto TERMINATE_PROGRAM;

TERMINATE_PROGRAM:
	std::cout << "\033[32m[SERVER CLOSED]\033[0m" << std::endl;
	return err_no;
}
