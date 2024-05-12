#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"

// Don't need this function in 'PASS' command
// void	Parser::pass(const std::vector< std::string > params)
// {
// 	//
// }

void	Command::pass(const IRC& server, int sd, const std::vector< std::string > params) throw (Error)
{
	Client* info;

	info = &(server._clients[sd]);

	// 이미 authorized 되었다면 무시한다
	if (info->getRegisterLevel() > Client::NONE)
		return ;

	// 그렇지 않다면 authorization을 수행한다
	if (server._password == params[0])
	{
		info->setRegisterLevel(Client::AUTHORIZED);
	}
	else
	{
		throw(Error::EAPASS);
	}
}
