#ifndef IRC_HPP
# define IRC_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <vector>
# include <map>
# include <string>
# include "Client.hpp"
# include "Channel.hpp"
# include "Error.hpp"

# define MAX_CLIENTS 128
# define MAX_EVENTS 10

class	Channel;

// SocketManager
class	IRC
{
	public:
		struct	AccessData
		{
			uint16_t	port;
			std::string	password;
		};

		static const char*	hostname;

		IRC(struct IRC::AccessData access_data);
		~IRC();
		void				boot();
		int					getServerSocketFd() const;
		const std::string&	getPassword() const;

		Channel*	searchChannel(std::string t_channel_name);
		Channel*	createChannel(Client& client, const char t_prefix, std::string t_channel_name);

	private:
		int			_server_sockfd;
		uint16_t	_port;
		const char*	_ip_addr;
		std::string	_password;

		std::vector<struct kevent>		_changelist;
		struct kevent					_eventlist[MAX_EVENTS];
		std::map<int, Client>			_clients;
		std::map<std::string, Channel>	_channels;

		void				setUpSocket() throw(Error);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Error);
};

#endif
