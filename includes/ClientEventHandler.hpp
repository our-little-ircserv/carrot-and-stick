#ifndef CLIENTEVENTHANDLER_HPP
# define CLIENTEVENTHANDLER_HPP

# include "IEventHandler.hpp"

class	ClientEventHandler : public IEventHandler
{
	public:
		virtual	~ClientEventHandler();

		virtual void	read(IRC& server, const struct kevent& event) throw(Signal, FatalError);
		virtual void	write(IRC& server, const struct kevent& event) throw(Signal, FatalError);

		void	handleEOF(IRC& server, Client& client) throw(enum Client::REGISTER_LEVEL);
};

#endif
