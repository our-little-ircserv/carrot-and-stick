#ifndef IRC_HPP
# define IRC_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <vector>
#include <map>
#include "Network.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class	IRC
{
	public:
		~IRC();

		void	runIRC(Network& network) throw(Signal, Error);
	
	private:
		std::vector<struct kevent>	m_changelist;
		struct kevent				m_eventlist[MAX_EVENTS];
		std::map<int, Client*>		m_clients;

		void		handleEvents(Network& network) throw(Signal, Error);
		void		acceptClient(Network& network) throw(Signal, Error);
		void		deleteClient(Client* client) throw(Signal, Error);
		void		handleMessages(struct kevent* event_occurred) throw(Signal, Error);
		std::string	receiveMessages(std::map<int, Client*>::iterator& it, struct kevent* event_occurred) throw(Signal, Error);
		void		sendMessages(Client* client) throw(Signal, Error);

		void	addReceivedMessagesToWriteBuffers(std::map<int, Client*>::iterator& msg_owner, Channel& channel);
		void	addReceivedMessagesToWriteBuffers(std::map<int, Client*>::iterator& msg_owner);
		void	moveReadBufferToWriteBuffer(std::vector<std::string>& rdbuf, std::vector<std::string>& wrbuf);
};

#endif
