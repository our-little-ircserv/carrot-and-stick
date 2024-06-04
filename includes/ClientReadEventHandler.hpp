#ifndef CLIENTREADEVENTHANDLER_HPP
# define CLIENTREADEVENTHANDLER_HPP

# include "IEventHandler.hpp"
# include "Client.hpp"

class	IRC;

class	ClientReadEventHandler : public IEventHandler
{
	public:
		virtual ~ClientReadEventHandler();

		virtual void	interpretEvent(IRC& server, const struct kevent& event);
	
	private:
		void	handleEOF(IRC& server, Client& client) throw(enum Client::REGISTER_LEVEL);
};

#endif
