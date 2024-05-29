#ifndef IEVENTHANDLER_HPP
# define IEVENTHANDLER_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include "IRC.hpp"
# include "Client.hpp"
# include "Signal.hpp"
# include "FatalError.hpp"

class	IRC;
class	Client;
class	Signal;
class	FatalError;

class	IEventHandler
{
	public:
		virtual	~IEventHandler();

		virtual void	read(IRC& server, const struct kevent& event) throw(Signal, FatalError) = 0;
		virtual void	write(IRC& server, const struct kevent& event) throw(Signal, FatalError) = 0;
};

#endif
