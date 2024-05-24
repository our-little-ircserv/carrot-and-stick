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
# include "FatalError.hpp"

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

		static const std::string	hostname;

		IRC(struct IRC::AccessData access_data);
		~IRC();

		void	init() throw(Signal, FatalError);
		void	run() throw(Signal, FatalError);

		Channel*	searchChannel(const std::string& channel_name);
		Channel*	createChannel(Client& client, const char prefix, const std::string& channel_name);

		Client*		searchClient(const int sockfd);
		Client*		searchClient(const std::string& nickname);

		void					disconnectClient(Client& client);
		std::vector< Channel* >	delClient(Client& client);
		void					delChannels(const std::vector< Channel* >& channels);

		int					getServerSocketFd() const;
		const std::string&	getPassword() const;

		void				deliverMsg(std::set< Client* >& target_list, std::string msg);
		std::set< Client* > getTargetSet(std::vector< std::string >targets);

	private:
		int			_server_sockfd;
		uint16_t	_port;
		const char*	_ip_addr;
		std::string	_password;

		std::vector< struct kevent >		_changelist;
		struct kevent						_eventlist[MAX_EVENTS];
		std::map< int, Client >				_clients;
		std::map< std::string, Channel >	_channels;

		void				setUpSocket() throw(Signal, FatalError);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Signal, FatalError);

		static void	get_next_line(Client& client, const std::string& input);
		static void	iterate_rdbuf(IRC& server, Client& client);

		static void	acceptClient(IRC& server, const struct kevent& event) throw(Signal, FatalError);
		static void	receiveMessages(IRC& server, const struct kevent& event) throw(Signal, FatalError);
		static void	sendMessages(IRC& server, const struct kevent& event) throw(Signal, FatalError);

		static void	handleEOF(IRC& server, Client& client) throw(enum Client::REGISTER_LEVEL);
};
//
//inline void	disconnectClient(IRC& server, Client& client)
//{
//	std::vector< Channel* >	empty_channels = server.delClient(client);
//	server.delChannels(empty_channels);
//}

#endif
