#include "Parser.hpp"
#include "Command.hpp"

struct Command::Join	Parser::join(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Join		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back("JOIN");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	const std::string&	t_channels = params[0];
	std::string			channel_name;
	size_t				i = 0;
	size_t				offset;
	size_t				t_channels_size = t_channels.size();
	while (i < t_channels_size)
	{
		offset = t_channels.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_channels_size;
		}
		offset -= i;

		if (offset > 50)
		{
			offset = 50;
		}

		channel_name = t_channels.substr(i, offset);

		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	if (params.size() == 1)
	{
		return data;
	}

	const std::string&	t_keys = params[1];
	size_t				t_keys_size = t_keys.size();
	std::string			key_value;
	i = 0;
	while (i < t_keys_size)
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
	struct Command::Join		p_data = Parser::join(data.parameters);
	std::vector< std::string >	r_params;
	size_t						chan_len = p_data.channels.size();

	for (size_t i = 0; i < chan_len; i++)
	{
		try
		{
			// r_params 를 초기화한다.
			r_params.clear();

			// 채널의 존재여부를 확인한다.
			Channel* channel = server.searchChannel(p_data.channels[i]);
			// 채널이 존재하지 않을경우...
			if (channel == NULL)
			{
				// 채널 이름이 문법에 적합하지 않으면 Reply 를 throw 한다.
				if (Parser::isValidChannelName(p_data.channels[i]) == false)
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
				}

				// 채널 이름이 문법에 적합하다면 새 채널을 생성한다.
				channel = server.createChannel(client, p_data.channels[i][0], p_data.channels[i]);
			}
			else
			{
				// 이미 채널에 존재하면 아무 동작도 하지 않는다.
				if (channel->isMember(client) == true)
				{
					return ;
				}

				// 채널에 들어가기 위한 유효성 검사를 수행한다.
				// 1. 초대여부
				if (channel->checkModeSet('i') == true && channel->isInvited(client) == false)
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_INVITEONLYCHAN, r_params);
				}
				// 2. 비밀번호
				if (channel->checkModeSet('k') == true && (i >= p_data.keys.size() || channel->getKey() != p_data.keys[i]))
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_BADCHANNELKEY, r_params);
				}
				// 3. 인원제한
				if (channel->checkModeSet('l') == true && channel->getLimit() <= channel->getMemberCnt())
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_CHANNELISFULL, r_params);
				}

				// 채널에 클라이언트를 추가한다.
				// +i 플래그가 활성화되었다면 초대권을 소모한다.
				channel->addMember(client);
				client.addChannelList(p_data.channels[i]);
				if (channel->isInvited(client) == true)
				{
					channel->delInvited(client);
				}
			}

			{
				// JOIN 메세지를 브로드캐스팅한다.
				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.push_back(p_data.channels[i]);
				r_params.push_back("\r\n");

				std::set< Client* > target_list = channel->getMemberSet();
				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}

			// TOPIC, NAMES 메세지를 위한 임시변수.
			struct Parser::Data t_data = data;
			t_data.parameters.clear();
			t_data.parameters.push_back(p_data.channels[i]);

			// 채널 접속시 해당 채널에 토픽이 있다면 표시한다.
			if (channel->getTopic().empty() == false)
			{
				Command::topic(server, client, t_data);
			}

			// 채널 접속시 해당 채널의 유저목록을 표시한다.
			Command::names(server, client, t_data);
		}
		catch(Reply& e)
		{
			// 채널 접속에 실패하거나 문법적 오류가 발생할경우 해당 클라이언트에게 Reply를 전달한다.
			server.deliverMsg(&client, e.getReplyMessage(client));
		}
	}
}
