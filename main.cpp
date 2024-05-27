#include "Assert.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "FatalError.hpp"
#include "IRC.hpp"
#include "Parser.hpp"
#include "Reply.hpp"
#include "Signal.hpp"

#include <iostream>
#include <sstream>

int kq = 0;
volatile sig_atomic_t g_signo = 0;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "argc error\n";
		return (1);
	}

	IRC::AccessData ad;
	std::stringstream ss(argv[1]);
	int a;
	ss >> a;
	std::cout << "port: " << a << "\n";
	ad.port = a;
	ad.password = argv[2];
	IRC server(ad);

	server.init();

	try
	{
		server.run();
	}
	catch(FatalError& e)
	{
		e.ftPerror();
	}
	
}