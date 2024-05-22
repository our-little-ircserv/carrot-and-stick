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

	if (data.parameters.size() < 1)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(data.command);
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	// 이미 authorized 되었다면 무시한다
	if (client.getRegisterLevel() == Client::REGISTERED)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_ALREADYREGISTRED, r_params);
	}

	// 그렇지 않다면 authorization을 수행한다
	if (server.getPassword() == data.parameters[0] && client.getRegisterLevel() == Client::NONE)
	{
		client.setRegisterLevel(Client::AUTHORIZED);
	}
	else if (client.getRegisterLevel() == Client::AUTHORIZED)
	{
		client.setRegisterLevel(Client::NONE);
	}
}
