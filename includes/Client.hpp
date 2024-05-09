#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/ip.h>
#include <arpa/inet.h>

# include <string>
# include <vector>

class	Client
{
	public:
		std::vector<std::string>	_read_buf;
		std::vector<std::string>	_write_buf;

		Client(int t_sockfd, struct sockaddr_in addr);

		int			getSocketFd() const;
		std::string	getPrefix() const;
		std::string	getHostname() const;
		void		setNickname(std::string t_nickname);
		std::string	getNickname() const;
		void		setUsername(std::string t_username);
		std::string	getUsername() const;
		void		setRealname(std::string t_realname);
		std::string	getRealname() const;

	private:
		int					_sockfd;
		struct sockaddr_in	_addr;
		std::string			_nickname;
		std::string			_username;
		std::string			_realname;

		Client();
};

#endif
