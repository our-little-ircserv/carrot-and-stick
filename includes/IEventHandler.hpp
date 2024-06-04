#ifndef IEVENTHANDLER_HPP
# define IEVENTHANDLER_HPP

class	IRC;

class	IEventHandler
{
	public:
		virtual ~IEventHandler();

		virtual void	interpretEvent(IRC& server, const struct kevent& event) = 0;
};

#endif
