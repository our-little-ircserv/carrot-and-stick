#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <netinet/ip.h>
# include <map>
# include "Client.hpp"
# include "Signal.hpp"
# include "Error.hpp"

class	Channel
{
	public:
		enum	Prefix
		{
			P_HASH = '#',
			P_AMPS = '&',
			P_PLUS = '+'
		};

		Channel(Client& client, enum Prefix prefix, std::string t_name, std::string t_modes);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);

		const std::string&	getChannelName() const;

		void				addMode(std::string mode_in_str);
		void				delMode(std::string mode_in_str);
		bool				checkModeSet(const char mode) const;
		const std::string	getCurrentMode() const;

		bool	isMember(Client& client) const;
		bool	isOperator(Client& client) const;
		void	addMember(Client& client);
		void	addOperator(Client& client);
		void	delMember(Client& client);
		void	delOperator(Client& client);

	private:
		static const std::string	st_valid_modes;

		const std::string		_name;
		size_t					_modes;
		std::string				_topic;
		std::string				_key;
		size_t					_limit;
		std::map<Client*, bool>	_members;
};

bool	operator<(const Client& a, const Client& b);
bool	operator>(const Client& a, const Client& b);

#endif
