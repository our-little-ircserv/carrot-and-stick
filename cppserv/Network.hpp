#ifndef NETWORK_HPP
# define NETWORK_HPP

# include "Error.hpp"

# define MAX_CLIENTS 128
# define MAX_EVENTS 10

// SocketManager
class	Network
{
	public:
		Network(uint16_t t_port);
		~Network();
		void	boot();
		int		getServerSocketFd() const;

	private:
		int			_server_sockfd;
		uint16_t	_port;
		const char*	_ip_addr;

		void				setUpSocket() throw(Error);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Error);
};

#endif
