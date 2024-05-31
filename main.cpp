#include <signal.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "FatalError.hpp"
#include "Parser.hpp"
#include "IRC.hpp"
#include "Signal.hpp"

volatile sig_atomic_t g_signo = 0;

void	handleSignals(int signo)
{
	g_signo = signo;
}

#include <unistd.h>
void	check_leaks(void)
{
	char	command[256];

	snprintf(command, sizeof(command), "%d", getpid());
	system(command);
}

int	main(int argc, char** argv)
{
	int	ret = 0;

//	atexit(check_leaks);
	try
	{
		IRC	network_manager(Parser::checkArgValidity(argc, argv));
		network_manager.init();
		signal(SIGINT, handleSignals);
		std::cout << "\033[32m[SERVER STARTED]\033[0m" << std::endl;
		network_manager.run();
	}
	catch (Signal& signal)
	{
		std::cout << "\033[2D";
	}
	catch (FatalError& err)
	{
		err.ftPerror();
		ret = err.getErrNo();
	}

	std::cout << "\033[32m[SERVER CLOSED]\033[0m" << std::endl;
	return ret;
}
