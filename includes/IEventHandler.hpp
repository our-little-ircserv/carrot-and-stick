#ifndef IEVENTHANDLER_HPP
# define IEVENTHANDLER_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include "Client.hpp"
# include "Signal.hpp"
# include "FatalError.hpp"

class	IRC;

class	IEventHandler
{
	public:
		virtual	~IEventHandler();

		virtual void	read(IRC& server, const struct kevent& event) throw(Signal, FatalError, enum Client::REGISTER_LEVEL) = 0;
		virtual void	write(IRC& server, const struct kevent& event) throw(Signal, FatalError) = 0;
};

#endif
