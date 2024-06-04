#include <iostream>
#include "Parser.hpp"
#include "Command.hpp"

struct Command::Part	Parser::part(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Part		data;
	std::vector< std::string >  r_params;

	size_t	params_size = params.size();
    if (params_size < 1)
	{
		r_params.push_back("PART");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	size_t	i = 0;
	size_t	offset;
	const std::string&	t_channels = params[0];
	size_t				t_channels_size = t_channels.size();
	std::string			channel_name;
	while (i < t_channels_size)
	{
		offset = t_channels.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_channels_size;
		}
		offset -= i;

		channel_name = t_channels.substr(i, offset);
		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	data.comment = ":";
    if (params_size > 1)
    {
		data.comment += params[1];
    }

	return data;
}

void	Command::part(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	struct Command::Part		p_data = Parser::part(data.parameters);
	size_t						channels_size = p_data.channels.size();
	std::vector< std::string >	r_params;
	std::vector< Channel* >		empty_channels;

	for (size_t i = 0; i < channels_size; i++)
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

			Command::handleClientDepartment(server, channel, client);
		}
		catch(Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);

			server.deliverMsg(target_list, e.getReplyMessage(client));
		}
	}
}
