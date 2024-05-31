#include <sstream>
#include "Channel.hpp"
#include "Parser.hpp"

const std::string	Channel::st_valid_modes = "itkl";

Channel::Channel() : _modes(0), _limit(0)
{
}

Channel::Channel(Client& client, const char t_prefix, std::string t_name) : _name(t_name), _modes(0), _limit(0)
{
	addMember(client);
	client.addChannelList(_name);

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

std::string	Channel::getCurrentMode() const
{
	std::string	current_mode = "+";

	for (std::string::const_iterator it = st_valid_modes.begin(); it != st_valid_modes.end(); it++)
	{
		if (checkModeSet(*it) == true)
		{
			current_mode += *it;
		}
	}

	return current_mode;
}

std::string	Channel::getCurrentModeParam(const bool key_auth) const
{
	std::vector< std::string >	current_mode_param;

	if (checkModeSet('k') == true && key_auth == true)
	{
		current_mode_param.push_back(_key);
	}

	if (checkModeSet('l') == true)
	{
		std::stringstream	ss;
		ss << _limit;
		current_mode_param.push_back(ss.str());
	}

	return Parser::concat_string_vector(current_mode_param);
}

const std::string&	Channel::getChannelName() const
{
	return _name;
}

const std::string&	Channel::getKey() const
{
	return _key;
}

size_t	Channel::getLimit() const
{
	return _limit;
}

size_t	Channel::getMemberCnt() const
{
	return _members.size();
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

// oitkl
void	Channel::setMode(struct Command::ModeWithParams& mode_data)
{
	size_t						shift = 0;
	std::vector< std::string >	r_params;
	if (mode_data.mode == 'o')
	{
		Client*	member = searchMember(mode_data.mode_param);
		if (member == NULL)
		{
			r_params.push_back(mode_data.mode_param);
			r_params.push_back(_name);
			throw Reply(Reply::ERR_USERNOTINCHANNEL, r_params);
		}
		else if (mode_data.type == Command::ADD)
		{
			addOperator(*member);
		}
		else
		{
			delOperator(*member);
		}
	}
	else if ((shift = st_valid_modes.find(mode_data.mode, 0)) != std::string::npos)
	{
		if (mode_data.type == Command::ADD)
		{
			if (setModeParam(mode_data.mode, mode_data.mode_param) == true)
			{
				_modes |= (1 << shift);
			}
		}
		else
		{
			_modes &= ~(1 << shift);
		}
	}
	else
	{
		std::vector< std::string >	r_params;

		r_params.push_back(std::string(1, mode_data.mode));
		r_params.push_back(_name);
		throw Reply(Reply::ERR_UNKNOWNMODE, r_params);
	}
}

// itkl 모드 세팅, shift 여부 판단.
bool	Channel::setModeParam(const char mode, const std::string& param)
{
	// i, t는 인자 없음. 무조건 shift.
	bool	shift_or_not = mode == 'i' || mode == 't';

	// k, l은 인자가 없거나 문제가 있으면 shift X.
	if (param.empty() == false)
	{
		if (mode == 'k')
		{
			_key = param;
			shift_or_not = true;
		}
		else if (mode == 'l')
		{
			size_t	t_limit = 0;
			std::stringstream	ss(param);
			ss >> t_limit;
			if (ss.fail() == false && ss.eof() == true && t_limit != 0)
			{
				_limit = t_limit;
				shift_or_not = true;
			}
		}
	}

	return shift_or_not;
}

void Channel::setTopic(const std::string& t_topic)
{
	_topic = t_topic;
}

bool	Channel::checkModeSet(const char mode) const
{
	size_t	mode_bit = (1 << st_valid_modes.find(mode, 0));

	return mode_bit == (_modes & mode_bit);
}

std::map<Client*, bool>::iterator Channel::getMemberBegin()
{
	return _members.begin();
}

std::map<Client*, bool>::iterator Channel::getMemberEnd()
{
	return _members.end();
}

std::set< Client* >	Channel::getMemberSet()
{
	std::set< Client* > target_list;

	std::map<Client*, bool>::iterator it = _members.begin();
	std::map<Client*, bool>::iterator ite = _members.end();

	for (; it != ite; it++)
	{
		target_list.insert(it->first);
	}

	return target_list;
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
	_invite_list.insert(&client);
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

bool	operator<(const Client& a, const Client& b)
{
	return a.getSocketFd() < b.getSocketFd();
}

bool	operator>(const Client& a, const Client& b)
{
	return a.getSocketFd() > b.getSocketFd();
}
