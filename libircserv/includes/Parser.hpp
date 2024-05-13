#ifndef PARSER_HPP
# define PARSER_HPP

# include <cinttypes>
# include <vector>
# include "IRC.hpp"
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

	struct	Data
	{
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	parameters;
	};

	struct IRC::AccessData	checkArgValidity(int argc, char** argv) throw(Error);
	
	struct Parser::Data		 			parseClientMessage(const std::string& message) throw(Error);
	std::vector<struct Parser::Token>	splitTokens(const std::string message);
	enum Parser::TokenType				extractTokenType(const std::string& token);

	struct Command::Join	join(const std::vector< std::string > params) throw(Error);
	std::string				nick(const std::vector< std::string > params) throw(Error);
	struct Command::Privmsg	privmsg(const std::vector< std::string > params);
	struct Command::User	user(const std::vector< std::string > params);
	struct	Command::Topic	topic(const std::vector< std::string > params);

	static bool			isAlpha(char c);
	static bool			isSpecial(char c);
	static bool			isDigit(char c);
	static bool			checkChannelPrefixes(char c);
	static bool			isValidChannelName(const std::string& channel_name);
	static bool			isValidUserName(const std::string& username);
	static std::string	concat_string_vector(const std::vector< std::string >& vec);
};

#endif
