#ifndef SERVER_H
# define SERVER_H

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>

# include <string>
# include <map>
# include <vector>

# include "Client.hpp"
# include "Channel.hpp"

# define MAX_EVENT 5

class Server
{
	public:
		Server();
		~Server();

		void init();
		void run();
	
	private:
		int							port;
		std::string					password;
		std::vector<struct kevent>	change_list;
		std::vector<struct kevent>	event_list;
		std::map<int, Client>		client_map;
		std::map<int, Channel>		channel_map;
};

#endif