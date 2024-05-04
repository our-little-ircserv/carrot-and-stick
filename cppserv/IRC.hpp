#ifndef IRC_HPP
# define IRC_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <vector>
#include <map>
#include "Network.hpp"
#include "Client.hpp"

class	IRC
{
	public:
		IRC();
		~IRC();

		void	runIRC(Network& network) throw(Signal, Error);
	
	private:
		std::vector<struct kevent>	changelist;
		std::vector<struct kevent>	eventlist;
		std::map<int, Client*>		clients;

		bool	writable;

		void		handleEvents(Network& network) throw(Signal, Error);
		void		acceptClient(Network& network) throw(Signal, Error);
		void		deleteClient(Client* client) throw(Signal, Error);
		void		handleMesssages(struct kevent* event_occurred) throw(Signal, Error);
		std::string	receiveMessages(std::map<int, Client*>::iterator& it, struct kevent* event_occurred) throw(Signal, Error);
		void		sendMessages(Client* client) throw(Signal, Error);

		void	addReceivedMessagesToWriteBuffers(std::map<int, Client*>::iterator& msg_owner);
		void	moveReadBufferToWriteBuffer(std::vector<std::string>& rdbuf, std::vector<std::string>& wrbuf);
};

#endif
