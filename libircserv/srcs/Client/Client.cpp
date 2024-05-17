#include "Client.hpp"

Client::Client()
{
	//
}

Client::Client(int t_sockfd, struct sockaddr_in t_addr) : _sockfd(t_sockfd), _addr(t_addr), _nickname("*")
{
	//
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

void Client::setNickname(std::string t_nickname)
{
	_nickname = t_nickname;
}

std::string Client::getNickname() const
{
	return _nickname;
}

void Client::setUsername(std::string t_username)
{
	_username = t_username;
}

std::string Client::getUsername() const
{
	return _username;
}

void Client::setRealname(std::string t_realname)
{
	_realname = t_realname;
}

std::string Client::getRealname() const
{
	return _realname;
}
