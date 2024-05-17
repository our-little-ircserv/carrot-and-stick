#include <limits.h>
#include <iostream>
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

//Tokens:
//-------
//:PREFIX
//COMMAND
//:PARAM PARAM
struct Parser::Data	Parser::parseClientMessage(const std::string& message) throw(FatalError)
{
	struct Parser::Data						command_data;
	std::vector<struct Parser::Token>			tokens = Parser::splitTokens(message);
	std::vector<struct Parser::Token>::iterator	it = tokens.begin();

	if (tokens.size() < 2)
	{
		throw FatalError(FatalError::EWRPARM, message.c_str());
	}

	if (it->type == Parser::COLON)
	{
		command_data.prefix = it->data;
		++it;
	}

	command_data.command = it->data;
	++it;

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

std::vector<struct Parser::Token>	Parser::splitTokens(const std::string message)
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

std::string	Parser::concat_string_vector(const std::vector< std::string >& vec)
{
	std::string	concat_params;

	size_t	i = 0;
	while (i < vec.size())
	{
		concat_params += vec[i];
		++i;

		if (i != vec.size())
		{
			concat_params += ' ';
		}
	}

	return concat_params;
}

//int	main()
//{
//	std::vector<struct Parser::Token> tokens = Parser::splitTokens(":PREFIX   COMMAND      hello world                        \r\n");
//	std::vector<struct Parser::Token> tokens = Parser::splitTokens(":PREFIX   COMMAND      :hello            world                        \r\n");
//
//	for (std::vector<struct Parser::Token>::iterator it = tokens.begin(); it != tokens.end(); it++)
//	{
//		std::cout << "Token - type: " << it->type << ", data: " << it->data << std::endl;
//	}
//
//	try
//	{
//		struct Parser::Data	command = Parser::parseClientMessage(":PREFIX    COMMAND    hello1\r\n");
//
//		std::cout << "Prefix: " << command.prefix << std::endl;
//		std::cout << "Command: " << command.command << std::endl;
//		for (std::vector<std::string>::iterator it = command.parameters.begin(); it != command.parameters.end(); it++)
//		{
//			std::cout << *it << std::endl;
//		}
//	}
//	catch (std::out_of_range& e)
//	{
//		std::cerr << e.what() << std::endl;
//	}
//}
