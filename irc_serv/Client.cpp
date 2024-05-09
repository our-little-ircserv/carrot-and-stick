#include "Client.hpp"

Client::Client(int t_sockfd, struct sockaddr_in addr) : _sockfd(t_sockfd), _host(addr)
{
	//
}

int Client::getSocketFd() const
{
	return (_sockfd);
}

// in the progress...
// std::string Client::getHostname() const
// {
// 	{
// 		struct sockaddr_in t_addr;
// 		socklen_t addr_len;

// 		addr_len = sizeof(t_addr);
// 		getsockname(_sockfd, (struct sockaddr*)&t_addr, &addr_len);
// 		//
// 	}

// 	{
// 		struct addrinfo hints, *res;
// 		memset(&hints, 0, sizeof(hints));
// 		hints.ai_family = AF_UNSPEC; // IPv4와 IPv6 허용
// 		getaddrinfo("www.example.com", "http", &hints, &res);
// 	}
// }

void Client::setNickname(std::string t_nickname)
{
	_nickname = t_nickname;
}

std::string Client::getNickname() const
{
	return (_nickname);
}

void Client::setUsername(std::string t_username)
{
	_username = t_username;
}

std::string Client::getUsername() const
{
	return (_username);
}

void Client::setRealname(std::string t_realname)
{
	_realname = t_realname;
}

std::string Client::getRealname() const
{
	return (_realname);
}