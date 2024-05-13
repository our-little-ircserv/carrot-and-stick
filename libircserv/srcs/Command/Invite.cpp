#include "Parser.hpp"
#include "Command.hpp"

struct Command::Invite	Parser::invite(const std::vector< std::string >& params) throw(Error)
{
	struct Command::Invite	data;

	if (params.size() < 2)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENEPARM, concat_params.c_str());
	}

	data.nickname = params[0];
	data.channel = params[1];

	return data;
}
