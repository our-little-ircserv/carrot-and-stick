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

	// 매개변수의 수를 검사합니다.
	if (data.parameters.size() < 1)
	{
		r_params.push_back(data.command);
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	// 이미 등록을 마친 상태라면 무시합니다.
	if (client.getRegisterLevel() == Client::REGISTERED)
	{
		throw Reply(Reply::ERR_ALREADYREGISTRED, r_params);
	}

	// 등록을 마치기전까진 비밀번호의 일치여부에 따라
	// 클라이언트의 등록 단계를 토글합니다.
	if (server.getPassword() == data.parameters[0] && client.getRegisterLevel() == Client::NONE)
	{
		client.setRegisterLevel(Client::AUTHORIZED);
	}
	else if (client.getRegisterLevel() == Client::AUTHORIZED)
	{
		client.setRegisterLevel(Client::NONE);
	}
}
