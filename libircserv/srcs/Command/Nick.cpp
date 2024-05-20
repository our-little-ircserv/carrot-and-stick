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

	new_nickname = Parser::nick(client, data.parameters);
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

	// broadcast on server
	// std::set< Client* > target_list;
	// {
	// 	const std::vector< std::string > chan_list = client.getChannelList();

	// 	for (size_t i = 0; i < chan_list.size(); i++)
	// 	{
	// 		Channel* channel = server.searchChannel(chan_list[i]);

	// 		std::map<Client*, bool>::iterator it = channel->getMemberBegin();
	// 		std::map<Client*, bool>::iterator ite = channel->getMemberEnd();

	// 		for (; it != ite; it++)
	// 		{
	// 			target_list.insert(it->first);
	// 		}
	// 	}
	// }
	// std::set< Client* > target_list;
	// std::set< Client* > t_list;
	// {
	// 	const std::vector< std::string > chan_list = client.getChannelList();

	// 	for (size_t i = 0; i < chan_list.size(); i++)
	// 	{
	// 		Channel* channel = server.searchChannel(chan_list[i]);
	// 		t_list = channel->getMemberSet();

	// 		target_list.insert(t_list.begin(), t_list.end());
	// 	}
	// }
	std::vector< std::string > target_names;
	target_names.push_back(client.getNickname());
	target_names.insert(target_names.end(), client.getChannelList().begin(), client.getChannelList().end());
	std::set< Client* > target_list = server.getTargetSet(target_names);

	r_params.push_back(data.prefix);
	r_params.push_back(data.command);
	r_params.insert(r_params.end(), data.parameters.begin(), data.parameters.end());

	server.deliveryMsg(target_list, Parser::concat_string_vector(r_params));
}
