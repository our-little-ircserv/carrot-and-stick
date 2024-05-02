#ifndef NETWORK_HPP
# define NETWORK_HPP

# include "Error.hpp"

// SocketManager
class	Network
{
	public:
		Network(uint16_t port);
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
