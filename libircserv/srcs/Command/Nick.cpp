#include <cctype>
#include "Parser.hpp"
#include "Command.hpp"

std::string	Parser::nick(const std::vector< std::string >& params) throw(Error)
{
	std::string			nickname;

	if (params.size() < 1)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENEPARM, concat_params.c_str());
	}

	const std::string&	t_nickname = params[0];
	char				c = t_nickname[0];
	if (Parser::isAlpha(c) == false \
			&& Parser::isSpecial(c) == false)
	{
		throw Error(Error::EWRPARM, t_nickname.c_str());
	}

	nickname += c;

	for (size_t i = 1; i < 9; i++)
	{
		c = t_nickname[i];
		if (Parser::isAlpha(c) == false && Parser::isSpecial(c) == false \
				&& Parser::isDigit(c) == false && c != '-')
		{
			throw Error(Error::EWRPARM, t_nickname.c_str());
		}

		nickname += c;
	}
	
	return nickname;
}
