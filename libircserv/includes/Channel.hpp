#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <netinet/ip.h>
# include <map>
# include <set>
# include "Client.hpp"
# include "Command.hpp"
# include "Signal.hpp"
# include "Error.hpp"

class	IRC;

class	Channel
{
	public:
		enum	Prefix
		{
			P_HASH = '#',
			P_AMPS = '&',
			P_PLUS = '+'
		};

		Channel();
		Channel(Client& client, const char t_prefix, std::string t_name);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);

		const std::string&	getChannelName() const;

		void				setMode(IRC& server, std::vector< struct Command::ModeWithParams>& mode_data);
		bool				checkModeSet(const char mode) const;
		const std::string	getCurrentMode() const;
		const std::string	getKey() const;
		const size_t		getLimit() const;
		const size_t		getMemberCnt() const;

		bool	isMember(Client& client) const;
		bool	isOperator(Client& client) const;
		bool	isInvited(Client& client) const;
		void	addMember(Client& client);
		void	addOperator(Client& client);
		void	addInvited(Client& client);
		void	delMember(Client& client);
		void	delOperator(Client& client);
		void	delInvited(Client& client);

	private:
		static const std::string	st_valid_modes;

		std::string				_name;
		size_t					_modes;
		std::string				_topic;
		std::string				_key;
		size_t					_limit;
		std::set<Client*>		_invite_list;
		std::map<Client*, bool>	_members;

		void	setAttributes(struct Command::ModeWithParams& mode_data);
};

bool	operator<(const Client& a, const Client& b);
bool	operator>(const Client& a, const Client& b);

#endif
