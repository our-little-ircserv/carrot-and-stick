#include "Parser.hpp"
#include "Command.hpp"

struct Command::Part	Parser::part(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Part		data;
	std::vector< std::string >  r_params;

    if (params.size() < 1)
	{
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

	data.comment = ":";
    if (params.size() > 1)
    {
		data.comment += params[1];
    }

	return data;
}

void	Command::part(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	struct Command::Part		p_data = Parser::part(data.parameters);
	size_t						chan_len = p_data.channels.size();
	std::vector< std::string >	r_params;
	std::vector< Channel* >		empty_channels;

	for (size_t i = 0; i < chan_len; i++)
	{
		try
		{
			r_params.clear();

            // 채널이 존재하는지 검사한다.
			Channel* channel = server.searchChannel(p_data.channels[i]);
			if (channel == NULL)
			{
				r_params.push_back(p_data.channels[i]);
				throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
			}

            // 클라이언트가 해당 채널에 존재하는지 검사한다.
            if (channel->isMember(client) == false)
            {
                r_params.push_back(p_data.channels[i]);
                throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
            }

			// part 메세지를 브로드캐스팅한다.
			{
				std::set< Client* > target_list = channel->getMemberSet();

				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.push_back(p_data.channels[i]);
                r_params.push_back(p_data.comment);
				r_params.push_back("\r\n");

				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}

			// 채널에서 해당 클라이언트를 제거한다.
			// 관리자와 멤버목록 모두에서 제거한다.
			if (channel->isOperator(client) == true)
            {
                channel->delOperator(client);
            }
            channel->delMember(client);
			// 클라이언트가 가지고있는 채널목록에서 해당 채널을 삭제한다.
            client.delChannelList(p_data.channels[i]);

			// 채널에 클라이언트가 더이상 남아있지 않으면
			// 빈 채널 목록에 추가한다.
			if (channel->getMemberCnt() == 0)
			{
				empty_channels.push_back(channel);
			}

		}
		catch(Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);

			server.deliverMsg(target_list, e.getReplyMessage(client));
		}
	}

	// 빈 채널을 서버에서 일괄적으로 삭제한다.
	server.delChannels(empty_channels);
}
