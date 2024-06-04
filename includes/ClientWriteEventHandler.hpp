#ifndef CLIENTWRITEEVENTHANDLER_HPP
# define CLIENTWRITEEVENTHANDLER_HPP

# include "IEventHandler.hpp"

class	IRC;

class	ClientWriteEventHandler : public IEventHandler
{
	public:
		virtual ~ClientWriteEventHandler();

		virtual void	interpretEvent(IRC& server, const struct kevent& event);
};

#endif
