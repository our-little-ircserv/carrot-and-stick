#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

# define MAX_MSGSIZE 514

class	Client
{
	public:
		std::vector<std::string>	_read_buf;
		std::vector<std::string>	_write_buf;

		Client(int t_sockfd);
		int	getSocketFd() const;
		int	getWritable() const;
		void	setWritable(bool t_writable);

	private:
		int		_sockfd;
		bool	_writable;
};

#endif
