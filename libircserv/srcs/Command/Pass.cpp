#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"

// Don't need this function in 'PASS' command
// void	Parser::pass(const std::vector< std::string > params)
// {
// 	//
// }

void	Command::pass(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	std::vector< std::string >	r_params;

	// 이미 authorized 되었다면 무시한다
	if (client.getRegisterLevel() > Client::AUTHORIZED)
		return ;

	// 그렇지 않다면 authorization을 수행한다
	if (server.getPassword() == data.parameters[0])
	{
		client.setRegisterLevel(Client::AUTHORIZED);
	}
	else
	{
//		throw(Error::EWRPARM);
	}
}
