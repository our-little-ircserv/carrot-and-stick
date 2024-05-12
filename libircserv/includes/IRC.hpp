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

// SocketManager
class	IRC
{
	public:
		IRC(uint16_t t_port);
		~IRC();
		void	boot();
		int		getServerSocketFd() const;

	private:
		int			_server_sockfd;
		uint16_t	_port;
		const char*	_ip_addr;
		std::string	_password;

		std::vector<struct kevent>	_changelist;
		struct kevent				_eventlist[MAX_EVENTS];
		std::map<int, Client>		_clients;
		std::map<int, Channel>		_channels;

		void				setUpSocket() throw(Error);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Error);
};

#endif
