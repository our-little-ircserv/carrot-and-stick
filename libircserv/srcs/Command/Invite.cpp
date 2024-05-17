#include "Parser.hpp"
#include "Command.hpp"

struct Command::Invite	Parser::invite(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Invite		data;
	std::vector< std::string >	r_params;

	if (params.size() < 2)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.nickname = params[0];
	data.channel = params[1];

	return data;
}
