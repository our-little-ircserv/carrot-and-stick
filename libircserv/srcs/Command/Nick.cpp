#include <cctype>
#include "Parser.hpp"
#include "Command.hpp"

std::string	Parser::nick(const Client& client, const std::vector< std::string >& params) throw(Error)
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

void Command::nick(IRC& server, Client& client, const std::vector< std::string >& params) throw (Error)
{
	std::string new_nickname;

	new_nickname = Parser::nick(client, params);
	// 인자 수는 유효하나 닉네임의 내용이 비어있을때
	// ERR_NONICKNAMEGIVEN
	if (new_nickname.size() == 0)
	{
		//
	}

	// 닉네임 형식에 맞지 않을 때 (Parser::nick에서 유효성 검사)
	// ERR_ERRONEUSNICKNAME

	// 이미 사용중인 닉네임일 때
	// ERR_NICKNAMEINUSE
	if (!(server.searchClient(new_nickname) == NULL))
	{
		//
	}

	client.setNickname(new_nickname);
}
