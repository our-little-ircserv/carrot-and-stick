#include <signal.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include "Network.hpp"
#include "Error.hpp"
#include "Parser.hpp"

volatile sig_atomic_t g_signo = 0;
std::string		password;
int	kq;

void	handleSignals(int signo)
{
	g_signo = signo;
}

void	handleEvents(Network& network) throw(Error)
{
	std::vector<struct kevent> changelist;
	std::vector<struct kevent> eventlist;
	struct kevent	event;
	struct timespec	time;
	time.tv_sec = 3;
	time.tv_nsec = 0;

	kq = wrapSyscall(kqueue(), "kqueue");
	EV_SET(&event, network.getServerSocketFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	changelist.push_back(event);
	while (true)
	{
		int	real_events = wrapSyscall(kevent(kq, &(*changelist.begin()), changelist.size(), &(*eventlist.begin()), MAX_EVENTS, &time), "kevent");
		std::cout << real_events << std::endl;
	}
}

int	main(int argc, char** argv)
{
	int	err_no = 0;
//	signal(SIGINT, SIG_IGN);

	try
	{
		Network	network(Parser::checkArgValidity(argc, argv));
		network.boot();
//		signal(SIGINT, handleSignals);
		handleEvents(network);
	}
	catch (Error& err)
	{
		err.ft_perror();
		err_no = err.getErrNo();
//
//		goto TERMINATE_PROGRAM;
	}

//TERMINATE_PROGRAM:
	std::cout << "\033[32m[SERVER CLOSED]\033[0m" << std::endl;
	return err_no;
}
