#ifndef PARSER_HPP
# define PARSER_HPP

# include <cinttypes>
# include <vector>
# include "Command.hpp"
# include "Error.hpp"

namespace	Parser
{
	enum	TokenType
	{
		COLON,
		NOCOLON,
	};

	struct	Token
	{
		enum Parser::TokenType	type;
		std::string				data;
	};

	uint16_t	checkArgValidity(int argc, char** argv) throw(Error);
	
	struct Command::Data				parseClientMessage(const std::string& message) throw(Error);
	std::vector<struct Parser::Token>	splitTokens(const std::string message);
	enum Parser::TokenType				extractTokenType(const std::string& token);

	struct Command::Join	join(const std::vector<std::string>& params) throw(Error);
};

#endif
