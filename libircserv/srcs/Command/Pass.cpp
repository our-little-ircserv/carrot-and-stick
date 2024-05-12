#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"

// Don't need this function in 'PASS' command
// void	Parser::pass(const std::vector< std::string > params)
// {
// 	//
// }

void	Command::pass(const IRC server, int sd, const std::vector< std::string > params) throw (Error)
{
	if (server._password == params[0])
	{
		// 이미 register 되었다면 무시한다
		// 그렇지 않다면 등록절차를 진행한다
		// nick, user 필요
	}
	else
	{
		throw(Error::EAPASS);
	}
}
