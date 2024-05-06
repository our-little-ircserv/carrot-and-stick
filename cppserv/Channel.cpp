#include <sys/socket.h>
#include <iostream>
#include <algorithm>
#include "Channel.hpp"

const std::string	Channel::st_valid_modes = "oitkl";

Channel::Channel(Client* client, enum Channel::Prefix prefix, std::string _name, std::string _modes) : m_name(_name), m_modes(0)
{
	addMember(client);

	if (prefix != P_PLUS)
		addOperator(client);

	addMode(_modes);
}

const std::string&	Channel::getChannelName() const
{
	return m_name;
}

void	Channel::addMode(std::string mode_in_str)
{
	size_t	shift = 0;

	for (std::string::iterator it = mode_in_str.begin() + 1; it != mode_in_str.end(); it++)
	{
		if ((shift = st_valid_modes.find(*it, 0)) != std::string::npos)
			m_modes |= (1 << shift);
	}
}

void	Channel::delMode(std::string mode_in_str)
{
	size_t	shift = 0;

	for (std::string::iterator it = mode_in_str.begin() + 1; it != mode_in_str.end(); it++)
	{
		if ((shift = st_valid_modes.find(*it, 0)) != std::string::npos)
			m_modes ^= (1 << shift);
	}
}

bool	Channel::checkModeSet(const char mode) const
{
	size_t	mode_bit = (1 << st_valid_modes.find(mode, 0));

	return mode_bit == (m_modes & mode_bit);
}

const std::string	Channel::getCurrentMode() const
{
	std::string	current_mode = "+";

	for (std::string::const_iterator it = st_valid_modes.begin(); it != st_valid_modes.end(); it++)
	{
		if (checkModeSet(*it) == true)
			current_mode.push_back(*it);
	}

	return current_mode;
}

void	Channel::addMember(Client* client)
{
	m_members.push_back(client);
}

void	Channel::addOperator(Client* client)
{
	if (isMember(client))
		m_operators.push_back(client);
}

bool	Channel::isMember(const Client* client) const
{
	return std::find(m_members.begin(), m_members.end(), client) != m_members.end();
}

bool	Channel::isOperator(const Client* client) const
{
	return std::find(m_operators.begin(), m_operators.end(), client) != m_operators.end();
}

void	Channel::sendMessageToMembers(const std::vector<std::string> message) const throw(Signal, Error)
{
	for (std::vector<std::string>::const_iterator it = message.begin(); it != message.end(); it++)
	{
		for (std::vector<Client*>::const_iterator jt = m_members.begin(); jt != m_members.end(); jt++)
		{
			wrapSyscall(send((*jt)->getSocketFd(), it->c_str(), it->size(), 0), "send");
		}
	}
}
//
//int	main()
//{
//	Client client(3);
//	Channel ch(&client, Channel::P_HASH, "#channel", "+tkil");
//	std::cout << ch.getCurrentMode() << std::endl;
//	ch.addMode("+ko");
//	std::cout << ch.getCurrentMode() << std::endl;
//	ch.delMode("-li");
//	std::cout << ch.getCurrentMode() << std::endl;
//
//	std::cout << std::boolalpha << ch.isOperator(&client) << std::endl;
//	std::cout << std::boolalpha << ch.isMember(&client) << std::endl;
//
//	Client yonghyle(2);
//	ch.addMember(&yonghyle);
//	std::cout << std::boolalpha << ch.isOperator(&yonghyle) << std::endl;
//	std::cout << std::boolalpha << ch.isMember(&yonghyle) << std::endl;
//}
