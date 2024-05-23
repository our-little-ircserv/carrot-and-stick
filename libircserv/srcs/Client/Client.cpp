#include <fcntl.h>
#include "Client.hpp"
#include "FatalError.hpp"

Client::Client()
{
	//
}

//Client::~Client()
//{
//	std::cout << "Client destroyed" << std::endl;
//}

Client::Client(int t_sockfd, const struct sockaddr_in& t_addr) : _sockfd(t_sockfd), _addr(t_addr), _writable(false), _nickname("*"), _register_level(Client::NONE)
{
	wrapSyscall(fcntl(_sockfd, F_SETFL, O_NONBLOCK), "fcntl");
}

int Client::getSocketFd() const
{
	return _sockfd;
}

std::string Client::getPrefix() const
{
	std::string ret;

	ret += ":";
	ret += getNickname();
	ret += "!";
	ret += getUsername();
	ret += "@";
	ret += getHostname();

	return ret;
}

std::string Client::getHostname() const
{
	return inet_ntoa(_addr.sin_addr);
}

int Client::getRegisterLevel() const
{
	return _register_level;
}

void Client::setRegisterLevel(int t_register_level)
{
	_register_level = t_register_level;
}

void Client::setNickname(const std::string& t_nickname)
{
	_nickname = t_nickname;
}

const std::string& Client::getNickname() const
{
	return _nickname;
}

void Client::setUsername(const std::string& t_username)
{
	_username = t_username;
}

const std::string& Client::getUsername() const
{
	return _username;
}

void Client::setRealname(const std::string& t_realname)
{
	_realname = t_realname;
}

const std::string& Client::getRealname() const
{
	return _realname;
}

void Client::addChannelList(const std::string& channel_name)
{
	std::vector< std::string >::iterator it = _w_channels.begin();
	std::vector< std::string >::iterator ite = _w_channels.end();

	for (; it != ite; it++)
	{
		if (*it == channel_name)
		{
			return ;
		}
	}
	_w_channels.push_back(channel_name);
}

void Client::delChannelList(const std::string& channel_name)
{
	std::vector< std::string >::iterator it = _w_channels.begin();
	std::vector< std::string >::iterator ite = _w_channels.end();

	for (; it != ite; it++)
	{
		if (*it == channel_name)
		{
			_w_channels.erase(it);
			return ;
		}
	}
}

const std::vector< std::string >& Client::getChannelList() const
{
	return _w_channels;
}
