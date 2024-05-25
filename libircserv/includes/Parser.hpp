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

	struct Command::Join		join(const Client& client, const std::vector< std::string >& params) throw(Reply);
	std::string					nick(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Privmsg		privmsg(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::User		user(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Topic		topic(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Invite		invite(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Kick		kick(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Mode		mode(const Client& client, const std::vector< std::string >& params) throw(Reply);
	struct Command::Part		part(const Client& client, const std::vector< std::string >& params) throw(Reply);
	std::vector< std::string >	names(const Client& client, const std::vector< std::string >& params) throw(Reply);

	std::string	concat_string_vector(const std::vector< std::string >& vec, const char identifier = ' ');

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
			if (username[i] == '@' || username[i] == '\n')
			{
				return false;
			}
			++i;
		}

		return true;
	}

	static enum Command::ModeType	getModeType(const char c)
	{
		if (c == '+')
		{
			return Command::ADD;
		}

		return Command::DEL;
	}

	static inline bool	isMode(const char c)
	{
		if (c == '+' || c == '-')
		{
			return true;
		}

		return false;
	}

	static size_t	insertModes(struct Command::Mode& data, const std::string& modes, char mtype)
	{
		struct Command::ModeWithParams	t_mode_with_params;

		size_t	i = 1;
		while (i < modes.size())
		{
			t_mode_with_params.type = Parser::getModeType(mtype);
			t_mode_with_params.mode = modes[i];
			data.modes.push_back(t_mode_with_params);
			++i;
		}

		return --i;
	}

	static size_t	insertModeParameters(struct Command::Mode& data, const std::vector< std::string >& params, size_t new_modes, size_t i)
	{
		std::vector< struct Command::ModeWithParams >::iterator it = data.modes.end() - new_modes;

		while (i < params.size() && it != data.modes.end())
		{
			if (it->mode != 'i' && it->mode != 't')
			{
				it->mode_param = params[i];
				++i;
			}

			++it;
		}

		while (i < params.size() && Parser::isMode(params[i][0]) == false)
		{
			++i;
		}
		
		return i;
	}
};

#endif
