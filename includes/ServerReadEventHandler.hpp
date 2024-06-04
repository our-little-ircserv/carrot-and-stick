#ifndef SERVERREADEVENTHANDLER_HPP
# define SERVERREADEVENTHANDLER_HPP

# include "IEventHandler.hpp"

class	IRC;

class	ServerReadEventHandler : public IEventHandler
{
	public:
		virtual ~ServerReadEventHandler();

		virtual void	interpretEvent(IRC& server, const struct kevent& event);
};

#endif
