#include "Parser.hpp"
#include "Command.hpp"

struct Command::Mode	Parser::mode(const std::vector< std::string >& params) throw(Error)
{
	struct Command::Mode	data;

	if (params.size() < 1)
	{
		throw Error(Error::ENEPARM, NULL);
	}

	data.channel = params[0];

	size_t	i = 1;
	while (i < params.size())
	{
		bool	mode = params[i][0] == '+' || params[i][0] == '-';
		if (mode == false)
		{
			break;
		}

		data.modes.push_back(params[i]);
		++i;
	}

	while (i < params.size())
	{
		data.mode_params.push_back(params[i]);
		++i;
	}

	return data;
}
