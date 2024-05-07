#include <sys/socket.h>
#include <iostream>
#include <algorithm>
#include "Channel.hpp"

const std::string	Channel::st_valid_modes = "oitkl";

Channel::Channel(Client* client, enum Channel::Prefix prefix, std::string _name, std::string _modes) : _name(_name), _modes(0)
{
	addMember(client);

	if (prefix != P_PLUS)
		addOperator(client);

	addMode(_modes);
}

const std::string&	Channel::getChannelName() const
{
	return _name;
}

void	Channel::addMode(std::string mode_in_str)
{
	size_t	shift = 0;

	for (std::string::iterator it = mode_in_str.begin() + 1; it != mode_in_str.end(); it++)
	{
		if ((shift = st_valid_modes.find(*it, 0)) != std::string::npos)
			_modes |= (1 << shift);
	}
}

void	Channel::delMode(std::string mode_in_str)
{
	size_t	shift = 0;

	for (std::string::iterator it = mode_in_str.begin() + 1; it != mode_in_str.end(); it++)
	{
		if ((shift = st_valid_modes.find(*it, 0)) != std::string::npos)
			_modes ^= (1 << shift);
	}
}

bool	Channel::checkModeSet(const char mode) const
{
	size_t	mode_bit = (1 << st_valid_modes.find(mode, 0));

	return mode_bit == (_modes & mode_bit);
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
	if (isMember(client) == false)
		_members.push_back(client);
}

bool	Channel::isMember(const Client* client) const
{
	return std::find(_members.begin(), _members.end(), client) != _members.end();
}

void	Channel::addOperator(Client* client)
{
	if (isMember(client) == true && isOperator(client) == false)
		_operators.push_back(client);
}

bool	Channel::isOperator(const Client* client) const
{
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

void	Channel::delMember(Client* client)
{
	if (isMember(client) == true)
	{
		delOperator(client);
		_members.erase(std::find(_members.begin(), _members.end(), client));
	}
}

void	Channel::delOperator(Client* client)
{
	if (isOperator(client) == true)
		_operators.erase(std::find(_operators.begin(), _operators.end(), client));
}
//
//void	Channel::sendMessageToMembers(const std::vector<std::string> message) const throw(Signal, Error)
//{
//	for (std::vector<std::string>::const_iterator it = message.begin(); it != message.end(); it++)
//	{
//		for (std::vector<Client*>::const_iterator jt = _members.begin(); jt != _members.end(); jt++)
//		{
//			wrapSyscall(send((*jt)->getSocketFd(), it->c_str(), it->size(), 0), "send");
//		}
//	}
//}
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
