#include <sys/socket.h>
#include <iostream>
#include "Channel.hpp"

const std::string	Channel::st_valid_modes = "itkl";

Channel::Channel(Client& client, enum Channel::Prefix prefix, std::string t_name) : _name(_name), _modes(0), _limit(0)
{
	addMember(client);

	if (prefix != P_PLUS)
		addOperator(client);
}
Channel::Channel(const Channel& other) : _name(other._name)
{
	*this = other;
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_modes = other._modes;
		_topic = other._topic;
		_key = other._key;
		_limit = other._limit;
		_members = other._members;
	}
	return *this
}

const std::string&	Channel::getChannelName() const
{
	return _name;
}

const std::string	Channel::getKey() const
{
	return _key;
}

const size_t		Channel::getLimit() const
{
	return _limit;
}

const size_t		Channel::getMemberCnt() const
{
	return _members.size();
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

bool	Channel::isMember(Client& client) const
{
	return _members.find(&client) != _members.end();
}

bool	Channel::isOperator(Client& client) const
{
	return isMember(client) == true && _members.find(&client)->second == true;
}

bool	Channel::isInvited(Client& client) const
{
	return _invite_list.find(&client) != _invite_list.end();
}

void	Channel::addMember(Client& client)
{
	if (isMember(client) == false)
		_members[&client] = false;
}

void	Channel::addOperator(Client& client)
{
	if (isOperator(client) == false)
	{
		_members[&client] = true;
	}
}

void	Channel::addInvited(Client& client)
{
	if (isMember(client) == false)
	{
		_invite_list.insert(&client);
	}
	else
	{
		// 에러처리
	}
}

void	Channel::delMember(Client& client)
{
	if (isMember(client) == true)
	{
		_members.erase(_members.find(&client));
	}
}

void	Channel::delOperator(Client& client)
{
	if (isOperator(client) == true)
	{
		_members[&client] = false;
	}
}

void	Channel::delInvited(Client& client)
{
	if (isInvited(client) == true)
	{
		_invite_list.erase(_invite_list.find(&client));
	}
}

bool	operator<(const Client& a, const Client& b)
{
	return a.getSocketFd() < b.getSocketFd();
}

bool	operator>(const Client& a, const Client& b)
{
	return a.getSocketFd() > b.getSocketFd();
}
