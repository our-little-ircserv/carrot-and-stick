#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <arpa/inet.h>
# include <string>
# include <vector>

class	Client
{
	public:
		enum REGISTER_LEVEL
		{
			NONE,
			AUTHORIZED,
			REGISTERED,
			LEFT
		};

		std::vector<std::string>	_read_buf;
		std::vector<std::string>	_write_buf;

		bool	_writable;

		Client();
		Client(int t_sockfd, const struct sockaddr_in& t_addr);

		int					getSocketFd() const;
		std::string			getPrefix() const;
		std::string			getHostname() const;
		int					getRegisterLevel() const;
		void				setRegisterLevel(int t_register_level);
		void				setNickname(const std::string& t_nickname);
		const std::string&	getNickname() const;
		void				setUsername(const std::string& t_username);
		const std::string&	getUsername() const;
		void				setRealname(const std::string& t_realname);
		const std::string&	getRealname() const;

		void								addChannelList(const std::string& channel_name);
		void								delChannelList(const std::string& channel_name);
		const std::vector< std::string >&	getChannelList() const;

	private:
		int							_sockfd;
		struct sockaddr_in			_addr;
		int							_register_level;
		std::string					_nickname;
		std::string					_username;
		std::string					_realname;
		std::vector< std::string >	_w_channels;
};

#endif
