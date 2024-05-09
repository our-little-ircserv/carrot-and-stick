#include <limits.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Error.hpp"
#include "Parser.hpp"

extern std::string		password;

uint16_t	Parser::checkArgValidity(int argc, char** argv) throw(Error)
{
	if (argc != 3)
	{
		throw Error(Error::EARGC, NULL);
	}

	std::string	port(argv[1]);
	std::istringstream iss(port);
	int	port_checker;
	iss >> port_checker;
	if (iss.fail() || !iss.eof() \
			|| port_checker < 0 || port_checker > USHRT_MAX)
	{
		throw Error(Error::EAPORT, NULL);
	}

	password = argv[2];
	return (uint16_t)port_checker;
}

//struct Command::Data	parseClientMessage(const std::string& message) throw(Error)
//{
//	std::vector<struct Parser::Token>	tokens = Parser::splitTokens(message);
//}

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

std::vector<struct Parser::Token>	Parser::splitTokens(const std::string message)
{
	std::vector<struct Parser::Token>	tokens;
	struct Parser::Token				t_token;
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
				offset = message.find_first_of(' ', i) - i;
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
//
//int	main()
//{
//	std::vector<struct Parser::Token> tokens = Parser::splitTokens(":PREFIX   COMMAND      hello world                        \r\n");
//	std::vector<struct Parser::Token> tokens = Parser::splitTokens(":PREFIX   COMMAND      :hello            world                        \r\n");
//
//	for (std::vector<struct Parser::Token>::iterator it = tokens.begin(); it != tokens.end(); it++)
//	{
//		std::cout << "Token - type: " << it->type << ", data: " << it->data << std::endl;
//	}
//}
