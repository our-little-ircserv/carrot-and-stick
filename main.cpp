#include <signal.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include "Error.hpp"
#include "Parser.hpp"
#include "IRC.hpp"
#include "Signal.hpp"

volatile sig_atomic_t g_signo = 0;
std::string		password;
int	kq;

void	handleSignals(int signo)
{
	g_signo = signo;
}

int	main(int argc, char** argv)
{
	int	err_no = 0;
	signal(SIGINT, SIG_IGN);

	try
	{
		IRC	network_manager(Parser::checkArgValidity(argc, argv));
		network_manager.boot();

		signal(SIGINT, handleSignals);
		std::cout << "\033[32m[SERVER STARTED]\033[0m" << std::endl;
	}
	catch (Signal& signal)
	{
		std::cout << "\033[2D";
	}
	catch (Error& err)
	{
		err.ftPerror();
		err_no = err.getErrNo();
	}

//TERMINATE_PROGRAM:
	std::cout << "\033[32m[SERVER CLOSED]\033[0m" << std::endl;
	return err_no;
}
