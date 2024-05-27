#include <cctype>
#include <algorithm>
#include "Parser.hpp"
#include "Command.hpp"

std::string	Parser::nick(const std::vector< std::string >& params) throw(Reply)
{
	std::string					nickname;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		throw Reply(Reply::ERR_NONICKNAMEGIVEN, r_params);
	}

	const std::string&	t_nickname = params[0];
	char				c = t_nickname[0];
	if (Parser::isAlpha(c) == false \
			&& Parser::isSpecial(c) == false)
	{
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

	// 서버의 비밀번호를 제대로 입력하지 않았다면 무시합니다.
	if (client.getRegisterLevel() == Client::NONE)
	{
		return ;
	}

	new_nickname = Parser::nick(data.parameters);

	// 이미 사용중인 닉네임인지 검사합니다.
	if ((server.searchClient(new_nickname) == NULL) == false)
	{
		r_params.push_back(new_nickname);
		throw Reply(Reply::ERR_NICKNAMEINUSE, r_params);
	}

	// 클라이언트의 닉네임을 변경합니다.
	client.setNickname(new_nickname);
	// 이미 등록을 마친 상태일경우
	// 접속해있는 모든 채널에 닉네임이 변경되었다는 메세지를 브로드캐스트합니다.
	if (client.getRegisterLevel() == Client::REGISTERED)
	{
		std::vector< std::string > target_names;
		target_names.push_back(client.getNickname());
		target_names.insert(target_names.end(), client.getChannelList().begin(), client.getChannelList().end());
		std::set< Client* > target_list = server.getTargetSet(target_names);

		r_params.push_back(data.prefix);
		r_params.push_back(data.command);
		r_params.insert(r_params.end(), data.parameters.begin(), data.parameters.end());
		r_params.push_back("\r\n");

		server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
	}
	// 등록이 완료된 직후
	// Reply 001 ~ 004 까지 해당 클라이언트에게 전달합니다.
	else if ((client.getUsername() == "") == false)
	{
		// 등록 단계를 REGISTERED로 설정합니다.
		client.setRegisterLevel(Client::REGISTERED);
		std::set< Client* > target_list;
		target_list.insert(&client);

		{
			Reply rp(Reply::RPL_WELCOME, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			Reply rp(Reply::RPL_YOURHOST, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			r_params.push_back(server.getStartTime());
			Reply rp(Reply::RPL_CREATED, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			Reply rp(Reply::RPL_MYINFO, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
	}
}
