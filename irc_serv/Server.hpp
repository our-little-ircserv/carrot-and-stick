#ifndef SERVER_HPP
# define SERVER_HPP

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
		int							_port;
		std::string					_password;
		std::vector<struct kevent>	_change_list;
		std::vector<struct kevent>	_event_list;
		std::map<int, Client>		_client_map;
		std::map<int, Channel>		_channel_map;
};

#endif