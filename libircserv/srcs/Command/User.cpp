#include "Parser.hpp"
#include "Command.hpp"

struct Command::User	user(const std::vector< std::string > params)
{
	struct Command::User	data;

	if (params.size() < 4)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENOEPARM, concat_params.c_str());
	}

	std::string	t_username = params[0];
	size_t	i = 0;
	while (i < t_username.size())
	{
		if (t_username[i] == '@')
		{
			throw Error(Error::EWRPARM, t_username.c_str());
		}
		++i;
	}

	data.username = t_username;
	data.realname = params[3];

	return data;
};
