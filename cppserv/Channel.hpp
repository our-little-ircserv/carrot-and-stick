#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
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

		Channel(Client* client, enum Prefix prefix, std::string _name, std::string _modes);

		const std::string&	getChannelName() const;

		void				addMode(std::string mode_in_str);
		void				delMode(std::string mode_in_str);
		bool				checkModeSet(const char mode) const;
		const std::string	getCurrentMode() const;

		void	addMember(Client* client);
		bool	isMember(const Client* client) const;
		void	addOperator(Client* client);
		bool	isOperator(const Client* client) const;

		void	sendMessageToMembers(const std::vector<std::string> message) const throw(Signal, Error);
	
	private:
		static const std::string	st_valid_modes;

		const std::string		m_name;
		size_t					m_modes;
		std::vector<Client*>	m_members;
		std::vector<Client*>	m_operators;
};

#endif
