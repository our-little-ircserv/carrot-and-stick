#include "Parser.hpp"
#include "Command.hpp"

struct	Command::Topic	Parser::topic(const std::vector< std::string >& params) throw(Error)
{
	struct	Command::Topic	data;

	if (params.size() < 1)
	{
		throw Error(Error::ENEPARM, Parser::concat_string_vector(params).c_str());
	}

	data.channel = params[0];
	if (params.size() > 1)
	{
		data.topic = params[1];
	}

	return data;
}
