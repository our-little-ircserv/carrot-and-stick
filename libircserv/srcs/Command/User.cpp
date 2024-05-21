#include "Parser.hpp"
#include "Command.hpp"

struct Command::User	Parser::user(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::User		data;
	std::vector< std::string >	r_params;

	if (params.size() < 4)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back("USER");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.username = params[0];
	data.real_name = params[3];

	return data;
};

void	Command::user(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	//
}
