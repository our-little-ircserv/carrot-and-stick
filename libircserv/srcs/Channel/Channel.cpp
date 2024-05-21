#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include "Channel.hpp"

const std::string	Channel::st_valid_modes = "itkl";

Channel::Channel() : _modes(0), _limit(0)
{
	//
}

Channel::Channel(Client& client, const char t_prefix, std::string t_name) : _name(t_name), _modes(0), _limit(0)
{
	addMember(client);

	if (static_cast< enum Prefix >(t_prefix) != P_PLUS)
	{
		addOperator(client);
	}
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
	return *this;
}

Client*	Channel::searchMember(const std::string& name)
{
	std::map<Client*, bool>::iterator it = _members.begin();
	std::map<Client*, bool>::iterator ite = _members.end();

	for (; it != ite; it++)
	{
		if (it->first->getNickname() == name)
		{
			return it->first;
		}
	}
	
	return NULL;
}

const std::string	Channel::getCurrentMode() const
{
	std::string	current_mode = "+";

	for (std::string::const_iterator it = st_valid_modes.begin(); it != st_valid_modes.end(); it++)
	{
		if (checkModeSet(*it) == true)
			current_mode += *it;
	}

	return current_mode;
}

const std::string&	Channel::getChannelName() const
{
	return _name;
}

const std::string&	Channel::getKey() const
{
	return _key;
}

const size_t	Channel::getLimit() const
{
	return _limit;
}

const size_t	Channel::getMemberCnt() const
{
	return _members.size();
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

void	Channel::setMode(const Client& client, std::vector< struct Command::ModeWithParams>& mode_data)
{
	size_t						shift = 0;
	std::vector< std::string >	r_params;
	std::vector< struct Command::ModeWithParams >::iterator it = mode_data.begin();
	std::vector< struct Command::ModeWithParams >::iterator ite = mode_data.end();

	for (; it != ite; it++)
	{
		if (it->mode == 'o')
		{
			Client*	member = searchMember(it->mode_param);
			if (member == NULL)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(it->mode_param);
				r_params.push_back(_name);
				throw Reply(Reply::ERR_USERNOTINCHANNEL, r_params);
			}
			else if (it->type == Command::ADD)
			{
				addOperator(*member);
			}
			else
			{
				delOperator(*member);
			}
		}
		else if ((shift = st_valid_modes.find(it->mode, 0)) != std::string::npos)
		{
			if (it->type == Command::ADD)
			{
				_modes |= (1 << shift);
				setAttributes(*it);
			}
			else
			{
				_modes ^= (1 << shift);
			}
		}
	}
}

void Channel::setTopic(std::string t_topic)
{
	_topic = t_topic;
}

bool	Channel::checkModeSet(const char mode) const
{
	size_t	mode_bit = (1 << st_valid_modes.find(mode, 0));

	return mode_bit == (_modes & mode_bit);
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
	_members[&client] = false;
}

void	Channel::addOperator(Client& client)
{
	_members[&client] = true;
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
	_members.erase(_members.find(&client));
}

void	Channel::delOperator(Client& client)
{
	_members[&client] = false;
}

void	Channel::delInvited(Client& client)
{
	_invite_list.erase(_invite_list.find(&client));
}

void	Channel::setAttributes(struct Command::ModeWithParams& mode_data)
{
	switch (mode_data.mode)
	{
		case 't':
			_topic = mode_data.mode_param;
			break;
		case 'k':
			_key = mode_data.mode_param;
			break;
		case 'l':
			std::istringstream(mode_data.mode_param) >> _limit;
			break;
		default:
			break;
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
