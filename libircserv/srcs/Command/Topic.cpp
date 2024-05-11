#include "Parser.hpp"
#include "Command.hpp"

struct	Command::Topic	Parser::topic(const std::vector< std::string > params)
{
	struct	Command::Topic	data;

	if (params.size() < 1)
	{
		throw Error(Error::ENOEPARM, concat_params.c_str());
	}

	const std::string&	t_channel = params[0];
	if (Parser::isValidChannelName(channel_name) == false)
	{
		throw Error(Error::EWRPARM, channel_name.c_str());
	}
}
