#ifndef NETWORK_HPP
# define NETWORK_HPP

# include "Error.hpp"

# define MAX_CLIENTS 128

// SocketManager
class	Network
{
	public:
		Network(uint16_t _port);
		~Network();
		void	boot();

		int	getServerSocketFd() const;

	private:
		int	server_sockfd;
		uint16_t	port;
		const char*	ip_addr;

		void	setUpSocket() throw(Error);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Error);
};

#endif
