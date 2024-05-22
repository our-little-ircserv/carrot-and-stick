#include "Parser.hpp"
#include "Command.hpp"

struct Command::Kick	Parser::kick(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Kick		data;
	std::vector< std::string >	r_params;

	if (params.size() < 2)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	size_t	i = 0;
	size_t	offset;
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

		channel_name = t_channels.substr(i, offset);
		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	const std::string&	t_users = params[1];
	std::string			user_nickname;
	i = 0;
	while (i < t_users.size())
	{
		offset = t_users.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_users.size();
		}
		offset -= i;

		user_nickname = t_users.substr(i, offset);
		data.users_nick.push_back(user_nickname);
		i += offset + 1;
	}

	size_t	channels_size = data.channels.size();
	if (channels_size > 1 && channels_size != data.users_nick.size())
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	if (params.size() > 2)
	{
		data.comment = params[2];
	}

	return data;
}

void	Command::kick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Kick		p_data = Parser::kick(client, data.parameters);
	size_t						chan_len = p_data.channels.size();
	size_t						client_len = p_data.users_nick.size();
	size_t						chan_idx = 0;
	size_t						client_idx = 0;
	std::vector< std::string >	r_params;

	while (chan_idx < chan_len && client_idx < client_len)
	{
		try
		{
			Channel*	channel = server.searchChannel(p_data.channels[chan_idx]);
			Client*		target_client = server.searchClient(p_data.users_nick[client_idx]);

			r_params.clear();
			// 채널이 존재하는지 검사
			// ERR_NOSUCHCHANNEL
			if (channel == NULL)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(p_data.channels[chan_idx]);
				throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
			}
			// 추방하는 자가 채널에 존재하는지 검사
			// ERR_NOTONCHANNEL
			else if (channel->isMember(client) == false)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(p_data.channels[chan_idx]);
				throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
			}
			// 추방하는 자가 채널 관리자인지 검사
			// ERR_CHANOPRIVSNEEDED
			else if (channel->isOperator(client) == false)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(p_data.channels[chan_idx]);
				throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
			}
			// 추방당하는 자가 채널에 존재하는지 검사
			// ERR_USERNOTINCHANNEL
			else if (channel->isMember(*target_client) == false)
			{
				r_params.push_back(client.getNickname());
				r_params.push_back(p_data.users_nick[chan_idx]);
				r_params.push_back(p_data.channels[client_idx]);
				throw Reply(Reply::ERR_USERNOTINCHANNEL, r_params);
			}

			// 채널에서 해당 클라이언트를 제거한다.
			// 관리자와 멤버목록 모두에서 제거한다.
			if (channel->isMember(*target_client) == true)
			{
				if (channel->isOperator(*target_client) == true)
				{
					channel->delOperator(*target_client);
				}
				channel->delMember(*target_client);
				target_client->delChannelList(p_data.channels[chan_idx]);
			}

			{
				std::set< Client* > target_list = channel->getMemberSet();

				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.push_back(p_data.channels[chan_idx]);
				r_params.push_back(p_data.users_nick[client_idx]);
				r_params.push_back(p_data.comment);
				r_params.push_back("\r\n");

				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}
		}
		catch(Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);
			server.deliverMsg(target_list, e.getReplyMessage());
		}

		client_idx++;
		// 채널 인자가 1개인경우
		// 하나의 채널에서 여러 클라이언트를 추방한다
		if (chan_len == 1)
		{
			continue;
		}
		chan_idx++;
	}
}
