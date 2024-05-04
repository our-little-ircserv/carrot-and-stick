#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

# define MAX_MSGSIZE 514

class	Client
{
	public:
		std::vector<std::string>	m_read_buf;
		std::vector<std::string>	m_write_buf;

		Client(int _sockfd);
		int	getSocketFd() const;
		int	getWritable() const;
		void	setWritable(bool _writable);

	private:
		int		m_sockfd;
		bool	m_writable;
};

#endif
