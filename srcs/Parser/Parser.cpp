#include <limits.h>
#include <sstream>
#include <string>
#include <exception>
#include "Parser.hpp"

struct IRC::AccessData	Parser::checkArgValidity(int argc, char** argv) throw(FatalError)
{
	if (argc != 3)
	{
		throw FatalError(FatalError::EARGC);
	}

	std::string	port(argv[1]);
	std::istringstream iss(port);
	int	port_checker;
	iss >> port_checker;
	if (iss.fail() || !iss.eof() \
			|| port_checker < 0 || port_checker > USHRT_MAX)
	{
		throw FatalError(FatalError::EAPORT);
	}

	struct IRC::AccessData	data_to_return;
	data_to_return.port = (uint16_t)port_checker;
	data_to_return.password = argv[2];

	return data_to_return;
}

struct Parser::Data	Parser::parseClientMessage(const std::string& message)
{
	struct Parser::Data							command_data;
	std::vector<struct Parser::Token>			tokens = Parser::splitTokens(message);
	std::vector<struct Parser::Token>::iterator	it = tokens.begin();

	if (it != tokens.end() && it->type == Parser::COLON)
	{
		command_data.prefix = it->data;
		++it;
	}

	if (it != tokens.end())
	{
		command_data.command = it->data;
		++it;
	}

	while (it != tokens.end())
	{
		command_data.parameters.push_back(it->data);
		++it;
	}

	return command_data;
}

static inline void	addDataToToken(std::string& token_data, std::vector<struct Parser::Token>& tokens)
{
	struct Parser::Token	t_token;

	if (!token_data.empty())
	{
		t_token.type = Parser::extractTokenType(token_data);
		t_token.data = token_data;
		tokens.push_back(t_token);
		token_data.clear();
	}
}

std::vector<struct Parser::Token>	Parser::splitTokens(const std::string& message)
{
	std::vector<struct Parser::Token>	tokens;
	std::string							t_token_data;

	size_t	i = 0;
	while (true)
	{
		if (message[i] == ' ')
		{
			addDataToToken(t_token_data, tokens);
			i = message.find_first_not_of(' ', i);
		}
		else if (message[i] == '\r')
		{
			addDataToToken(t_token_data, tokens);
			break;
		}
		else
		{
			size_t	offset;
			if (message[i] == ':' && i != 0)
			{
				offset = message.find_first_of('\r', i) - i;
			}
			else
			{
				offset = message.find_first_of(" \r", i) - i;
			}
			t_token_data = message.substr(i, offset);
			i += offset;
		}
	}

	return tokens;
}

enum Parser::TokenType	Parser::extractTokenType(const std::string& token)
{
	if (*(token.begin()) == ':')
	{
		return Parser::COLON;
	}

	return Parser::NOCOLON;
}

std::string	Parser::concat_string_vector(const std::vector< std::string >& vec, const char identifier)
{
	std::string	concat_params;

	size_t	i = 0;
	while (i < vec.size())
	{
		concat_params += vec[i];
		++i;

		if (i != vec.size())
		{
			concat_params += identifier;
		}
	}

	return concat_params;
}

bool	Parser::isAlpha(char c)
{
	return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool	Parser::isSpecial(char c)
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

bool	Parser::isDigit(char c)
{
	return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool	Parser::checkChannelPrefixes(char c)
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

bool	Parser::isValidChannelName(const std::string& channel_name)
{
	if (channel_name.size() < 2 || checkChannelPrefixes(channel_name[0]) == false)
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

bool	Parser::isValidUserName(const std::string& username)
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

enum Command::ModeType	Parser::getModeType(const char c)
{
	if (c == '+')
	{
		return Command::ADD;
	}

	return Command::DEL;
}

bool	Parser::isMode(const char c)
{
	if (c == '+' || c == '-')
	{
		return true;
	}

	return false;
}

size_t	Parser::insertModes(struct Command::Mode& data, const std::string& modes, char mtype)
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

size_t	Parser::insertModeParameters(struct Command::Mode& data, const std::vector< std::string >& params, size_t new_modes, size_t i)
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
