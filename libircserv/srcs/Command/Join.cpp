#include "Parser.hpp"
#include "Command.hpp"

struct Command::Join	Parser::join(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Join		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back("JOIN");
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

		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	if (params.size() == 1)
	{
		return data;
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
	struct Command::Join		p_data = Parser::join(client, data.parameters);
	std::vector< std::string >	r_params;
	size_t						chan_len = p_data.channels.size();

	for (size_t i = 0; i < chan_len; i++)
	{
		try
		{
			r_params.clear();

			Channel* channel = server.searchChannel(p_data.channels[i]);	
			if (channel == NULL)
			{
				if (Parser::isValidChannelName(p_data.channels[i]) == false)
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
				}

				channel = server.createChannel(client, p_data.channels[i][0], p_data.channels[i]);
			}
			else
			{
				// 이미 채널에 존재하면 아무 동작도 하지 않습니다.
				if (channel->isMember(client) == true)
				{
					return ;
				}

				// 채널에 들어가기 위한 유효성 검사

				// 1. 초대여부
				if (channel->checkModeSet('i') == true && channel->isInvited(client) == false)
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_INVITEONLYCHAN, r_params);
				}
				// 2. 비밀번호
				else if (channel->checkModeSet('k') == true && (channel->getKey() == p_data.keys[i]) == false)
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_BADCHANNELKEY, r_params);
				}
				// 3. 인원제한
				else if (channel->checkModeSet('l') == true && channel->getLimit() == channel->getMemberCnt())
				{
					r_params.push_back(p_data.channels[i]);
					throw Reply(Reply::ERR_CHANNELISFULL, r_params);
				}

				channel->addMember(client);
				client.addChannelList(p_data.channels[i]);
				if (channel->isInvited(client) == true)
				{
					channel->delInvited(client);
				}
			}

			{
				std::set< Client* > target_list = channel->getMemberSet();

				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.push_back(p_data.channels[i]);
				r_params.push_back("\r\n");

				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}

			struct Parser::Data t_data = data;
			t_data.parameters.clear();
			t_data.parameters.push_back(p_data.channels[i]);
			Command::names(server, client, data);
		}
		catch(Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);

			r_params.push_back("\r\n");
			server.deliverMsg(target_list, e.getReplyMessage(client));
		}
	}
}
