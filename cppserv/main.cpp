#include <signal.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include "Network.hpp"
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
		Network	network(Parser::checkArgValidity(argc, argv));
		network.boot();
		signal(SIGINT, handleSignals);
		IRC event_handler;
		event_handler.handleEvents(network);
	}
	catch (Signal& signal)
	{
		std::cout << "\033[2D";
//		int	received_signal = signal.getSignal();
//		if (received_signal == SIGINT || received_signal == SIGQUIT)
//			goto TERMINATE_PROGRAM;
	}
	catch (Error& err)
	{
		err.ft_perror();
		err_no = err.getErrNo();

//		goto TERMINATE_PROGRAM;
	}

//TERMINATE_PROGRAM:
	std::cout << "\033[32m[SERVER CLOSED]\033[0m" << std::endl;
	return err_no;
}
