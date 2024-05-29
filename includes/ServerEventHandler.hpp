#ifndef SERVEREVENTHANDLER_HPP
# define SERVEREVENTHANDLER_HPP

# include "IEventHandler.hpp"

class	ServerEventHandler : public IEventHandler
{
	public:
		virtual	~ServerEventHandler();

		virtual void	read(IRC& server, const struct kevent& event) throw(Signal, FatalError);
		virtual void	write(IRC& server, const struct kevent& event) throw(Signal, FatalError);
};

#endif
