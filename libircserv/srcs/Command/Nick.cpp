#include <cctype>
#include <algorithm>
#include "Parser.hpp"
#include "Command.hpp"

std::string	Parser::nick(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	std::string					nickname;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NONICKNAMEGIVEN, r_params);
	}

	const std::string&	t_nickname = params[0];
	char				c = t_nickname[0];
	if (Parser::isAlpha(c) == false \
			&& Parser::isSpecial(c) == false)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(t_nickname);
		throw Reply(Reply::ERR_ERRONEUSNICKNAME, r_params);
	}

	nickname += c;

	size_t	nick_size = std::min(t_nickname.size(), (size_t)9);
	for (size_t i = 1; i < nick_size; i++)
	{
		c = t_nickname[i];
		if (Parser::isAlpha(c) == false && Parser::isSpecial(c) == false \
				&& Parser::isDigit(c) == false && c != '-')
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(t_nickname);
			throw Reply(Reply::ERR_ERRONEUSNICKNAME, r_params);
		}

		nickname += c;
	}
	
	return nickname;
}

void Command::nick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	std::string					new_nickname;
	std::vector< std::string >	r_params;

	if (client.getRegisterLevel() == Client::NONE)
	{
		return ;
	}

	new_nickname = Parser::nick(client, data.parameters);
	// 이미 사용중인 닉네임일 때
	if ((server.searchClient(new_nickname) == NULL) == false)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(new_nickname);
		throw Reply(Reply::ERR_NICKNAMEINUSE, r_params);
	}

	client.setNickname(new_nickname);
	if (client.getRegisterLevel() == Client::REGISTERED)
	{
		std::vector< std::string > target_names;
		target_names.push_back(client.getNickname());
		target_names.insert(target_names.end(), client.getChannelList().begin(), client.getChannelList().end());
		std::set< Client* > target_list = server.getTargetSet(target_names);

		r_params.push_back(data.prefix);
		r_params.push_back(data.command);
		r_params.insert(r_params.end(), data.parameters.begin(), data.parameters.end());

		server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
	}
	// 환영합니다
	else if ((client.getUsername() == "") == false)
	{
		client.setRegisterLevel(Client::REGISTERED);
		std::set< Client* > target_list;
		target_list.insert(&client);
		r_params.push_back(client.getNickname());

		{
			Reply rp(Reply::RPL_WELCOME, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_YOURHOST, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_CREATED, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_MYINFO, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
	}
}
