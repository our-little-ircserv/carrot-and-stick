#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

# define MAX_MSGSIZE 514

class	Client
{
	public:
		std::vector<std::string>	read_buf;
		std::vector<std::string>	write_buf;

		Client(int _sockfd);
		int	getSocketFd() const;

	private:
		int	sockfd;
};

#endif
