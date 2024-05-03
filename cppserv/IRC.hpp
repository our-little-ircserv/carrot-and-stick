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

		void	handleEvents(Network& network);
	
	private:
		std::vector<struct kevent>	changelist;
		std::vector<struct kevent>	eventlist;
		std::map<int, Client*>		clients;

		void		acceptClient(Network& network);
		void		recvSendMesssages(struct kevent* event_occurred);
		std::string	receiveMessages(struct kevent* event_occurred);
};

#endif
