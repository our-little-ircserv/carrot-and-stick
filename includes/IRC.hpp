#ifndef IRC_HPP
# define IRC_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <ctime>
# include <vector>
# include <map>
# include <string>
# include "ServerReadEventHandler.hpp"
# include "ClientReadEventHandler.hpp"
# include "ClientWriteEventHandler.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "FatalError.hpp"

# define MAX_CLIENTS 128
# define MAX_EVENTS 100
# define BUFSIZE 1025

class	Channel;
class	ServerEventHandler;
class	ClientEventHandler;

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

		void	pushEvent(const struct kevent& event);

		Channel*	searchChannel(const std::string& channel_name);
		Channel*	createChannel(Client& client, const char prefix, const std::string& channel_name);

		Client*		searchClient(const int sockfd);
		Client*		searchClient(const std::string& nickname);
		Client&		createClient(const int sockfd, const struct sockaddr_in& addr);

		void	delClient(const Client& client);
		void	delChannel(const Channel* channel);

		std::string					getStartTime() const;
		int							getServerSocketFd() const;
		const std::string&			getPassword() const;
		ClientReadEventHandler&		getClientReadEventHandler();
		std::vector< Channel* >		getEmptyChannels(const std::vector< std::string >& chan_list);
		std::set< Client* >			getTargetSet(const std::vector< std::string >& targets);

		void				deliverMsg(const std::set< Client* >& target_list, const std::string& msg);
		void				deliverMsg(Client* target, const std::string& msg);

		static void	get_next_line(Client& client, const std::string& input);
		static void	iterate_rdbuf(IRC& server, Client& client);

	private:
		std::tm*	_start_time;
		int			_server_sockfd;
		uint16_t	_port;
		const char*	_ip_addr;
		std::string	_password;

		std::vector< struct kevent >	_changelist;
		struct kevent					_eventlist[MAX_EVENTS];

		ServerReadEventHandler	_server_read_handler;
		ClientReadEventHandler	_client_read_handler;
		ClientWriteEventHandler	_client_write_handler;

		std::map< int, Client >				_clients;
		std::map< std::string, Channel >	_channels;

		void				setUpSocket() throw(Signal, FatalError);
		struct sockaddr_in	setSockAddrIn(int domain) throw(Signal, FatalError);
};

#endif
