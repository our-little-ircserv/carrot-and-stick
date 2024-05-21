#include "Parser.hpp"
#include "Command.hpp"
#include <iostream>

struct Command::Join	Parser::join(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Join		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	size_t				i = 0;
	size_t				offset;
	const std::string&	t_channels = params[0];
	std::string			channel_name;
	while (i < t_channels.size())
	{
		offset = t_channels.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_channels.size();
		}
		offset -= i;

		if (offset > 50)
		{
			offset = 50;
		}

		channel_name = t_channels.substr(i, offset);
		if (Parser::isValidChannelName(channel_name) == false)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(channel_name);
			throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
		}

		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	const std::string&	t_keys = params[1];
	std::string			key_value;
	i = 0;
	while (i < t_keys.size())
	{
		offset = t_keys.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_keys.size();
		}
		offset -= i;

		key_value = t_keys.substr(i, offset);
		data.keys.push_back(key_value);
		i += offset + 1;
	}

	return data;
}

void	Command::join(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	struct Command::Join		p_data;
	std::vector< std::string >	r_params;

	p_data = ::Parser::join(client, data.parameters);
	/*

	채널목록을 순회하며
	1. 채널이 존재하는지 확인한다, 없다면 새로 생성한다. (이름이 여럿일때 유효하지 않은 이름이 섞여있는 경우 동작 확인할것)
	2. 채널의 모드를 확인한다.(모드간 우선순위 확인할것)
	3. 모드에 따른 확인 절차를 거치고 멤버를 추가한다.

	isExistChannel
	true -> getChannel -> reference? 초기화가 아니면 값 대입이 불가능 -> pointer*

	*/
	size_t chan_len;

	chan_len = p_data.channels.size();

	for (size_t i = 0; i < chan_len; i++)
	{
		try
		{
			r_params.clear();

			if (Parser::isValidChannelName(p_data.channels[i]) == false)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(p_data.channels[i]);
				throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
			}
			Channel* channel = server.searchChannel(p_data.channels[i]);

			if (channel == NULL)
			{
				channel = server.createChannel(client, p_data.channels[i][0], p_data.channels[i]);
				{
					std::set< Client* > target_list = channel->getMemberSet();

					r_params.push_back(data.prefix);
					r_params.push_back(data.command);
					r_params.push_back(p_data.channels[i]);

					server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
				}
				continue ;
			}

			// 채널에 들어가기 위한 유효성 검사

			// 1. 초대여부
			if (channel->checkModeSet('i') == true)
			{
				if (channel->isInvited(client) == false)
				{
					r_params.push_back(client.getNickname());
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_USERONCHANNEL, r_params);
				}
			}

			// 2. 비밀번호
			if (channel->checkModeSet('k') == true)
			{
				if ((channel->getKey() == p_data.keys[i]) == false)
				{
					r_params.push_back(client.getNickname());
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_BADCHANNELKEY, r_params);
				}
			}

			// 3. 인원제한
			if (channel->checkModeSet('l') == true)
			{
				if (channel->getLimit() == channel->getMemberCnt())
				{
					r_params.push_back(client.getNickname());
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_CHANNELISFULL, r_params);
				}
			}

			channel->addMember(client);

			{
				std::set< Client* > target_list = channel->getMemberSet();

				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.push_back(p_data.channels[i]);

				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}
		}
		catch(Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);
			server.deliverMsg(target_list, e.getReplyMessage());
		}
	}
}
