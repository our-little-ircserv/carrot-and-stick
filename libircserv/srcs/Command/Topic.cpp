#include "Parser.hpp"
#include "Command.hpp"

struct	Command::Topic	Parser::topic(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct	Command::Topic		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back("TOPIC");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.channel = params[0];
	if (params.size() > 1)
	{
		data.topic = params[1];
	}

	return data;
}
