#ifndef PARSER_HPP
# define PARSER_HPP

# include <cinttypes>
# include <vector>
# include "IRC.hpp"
# include "Command.hpp"
# include "FatalError.hpp"
# include "Reply.hpp"

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
		std::vector< std::string >	parameters;
	};

	struct IRC::AccessData	checkArgValidity(int argc, char** argv) throw(FatalError);
	
	struct Parser::Data		 			parseClientMessage(const std::string& message);
	std::vector<struct Parser::Token>	splitTokens(const std::string message);
	enum Parser::TokenType				extractTokenType(const std::string& token);

	struct Command::Join		join(const std::vector< std::string >& params) throw(Reply);
	std::string					nick(const std::vector< std::string >& params) throw(Reply);
	struct Command::Privmsg		privmsg(const std::vector< std::string >& params) throw(Reply);
	struct Command::User		user(const std::vector< std::string >& params) throw(Reply);
	struct Command::Topic		topic(const std::vector< std::string >& params) throw(Reply);
	struct Command::Invite		invite(const std::vector< std::string >& params) throw(Reply);
	struct Command::Kick		kick(const std::vector< std::string >& params) throw(Reply);
	struct Command::Mode		mode(const std::vector< std::string >& params) throw(Reply);
	struct Command::Part		part(const std::vector< std::string >& params) throw(Reply);
	std::vector< std::string >	names(const std::vector< std::string >& params) throw(Reply);

	std::string	concat_string_vector(const std::vector< std::string >& vec, const char identifier = ' ');

	bool	isAlpha(char c);
	bool	isSpecial(char c);
	bool	isDigit(char c);
	bool	isValidChannelName(const std::string& channel_name);
	bool	isValidUserName(const std::string& username);
	bool	isMode(const char c);

	bool	checkChannelPrefixes(char c);

	enum Command::ModeType	getModeType(const char c);

	size_t	insertModes(struct Command::Mode& data, const std::string& modes, char mtype);
	size_t	insertModeParameters(struct Command::Mode& data, const std::vector< std::string >& params, size_t new_modes, size_t i);
	
};

#endif
