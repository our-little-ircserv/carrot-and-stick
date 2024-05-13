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

	struct	Data
	{
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	parameters;
	};

	uint16_t	checkArgValidity(int argc, char** argv) throw(Error);
	
	struct Parser::Data		 			parseClientMessage(const std::string& message) throw(Error);
	std::vector<struct Parser::Token>	splitTokens(const std::string message);
	enum Parser::TokenType				extractTokenType(const std::string& token);

	struct Command::Join	join(const std::vector< std::string > params) throw(Error);
	std::string				nick(const std::vector< std::string > params) throw(Error);
	struct Command::Privmsg	privmsg(const std::vector< std::string > params);
	struct Command::User	user(const std::vector< std::string > params);
	struct	Command::Topic	topic(const std::vector< std::string > params);

	static bool	isAlpha(char c)
	{
		return std::isalpha(static_cast<unsigned char>(c)) != 0;
	}

	static bool	isSpecial(char c)
	{
		const char*	special_characters = "[]\\`_^{|}";

		size_t	i = 0;
		while (special_characters[i] != '\0')
		{
			if (c == special_characters[i])
			{
				return true;
			}
			++i;
		}

		return false;
	}

	static bool	isDigit(char c)
	{
		return std::isdigit(static_cast<unsigned char>(c)) != 0;
	}

	static bool	checkChannelPrefixes(char c)
	{
		const char*	prefixes = "#&+";

		size_t	i = 0;
		while (prefixes[i] != '\0')
		{
			if (c == prefixes[i])
			{
				return true;
			}
			++i;
		}
		
		return false;
	}

	static bool	isValidChannelName(const std::string& channel_name)
	{
		if (checkChannelPrefixes(channel_name[0]) == false)
		{
			return false;
		}

		size_t	i = 1;
		while (channel_name[i] != '\0')
		{
			char c = channel_name[i];
			if (c == ' ' || c == '\a' || c == ',' || c == ':')
			{
				return false;
			}
			++i;
		}

		return true;
	}

	static bool	isValidUserName(const std::string& username)
	{
		size_t	i = 0;
		while (i < username.size())
		{
			if (t_username[i] == '@' || t_username[i] == '\n')
			{
				return false;
			}
			++i;
		}

		return true;
	}

	static	std::string	concat_string_vector(const std::vector< std::string >& vec)
	{
		std::string	concat_params;

		size_t	i = 0;
		while (i < vec.size())
		{
			concat_params += vec[i];
			++i;
		}

		return concat_params;
	}
};

#endif
