#include "Parser.hpp"
#include "Command.hpp"

std::vector< std::string >	Parser::names(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	std::vector< std::string >	data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back("NAMES");
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

		channel_name = t_channels.substr(i, offset);
		data.push_back(channel_name);
		i += offset + 1;
	}

	return data;
}

void	Command::names(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	std::vector< std::string >	channels = Parser::names(client, data.parameters);
	size_t						chan_len = channels.size();
	size_t						chan_idx = 0;
	std::vector< std::string >	r_params;
	std::set< Client* >			target_list;


	// 채널의 존재여부나 이름의 유효성은 검사하지 않는다
	target_list.insert(&client);
	r_params.push_back(client.getNickname());
	while (chan_idx < chan_len)
	{
		r_params.resize(1);
		try
		{
			Channel*	channel = server.searchChannel(channels[chan_idx]);

			if ((channel == NULL) == false && channel->isMember(client) == true)
			{
				r_params.push_back("=");
				r_params.push_back(channels[chan_idx]);

				std::map<Client*, bool>::iterator it = channel->getMemberEnd();
				std::map<Client*, bool>::iterator ite = channel->getMemberBegin();
				std::vector< std::string > nick_list;
				std::string t_nick;

				--it;
				while (true)
				{
					t_nick.clear();
					if (channel->isOperator(*it->first))
					{
						t_nick += "@";
					}
					t_nick += it->first->getNickname();
					nick_list.push_back(t_nick);
					if (it == ite)
					{
						break ;
					}
					--it;
				}
				r_params.push_back(":" + Parser::concat_string_vector(nick_list));
				throw Reply(Reply::RPL_NAMREPLY, r_params);
			}
		}
		catch(Reply& e)
		{
			server.deliverMsg(target_list, e.getReplyMessage());
		}

		r_params.resize(1);
		r_params.push_back(channels[chan_idx]);
		r_params.push_back(":End of NAMES list");

		server.deliverMsg(target_list, Reply(Reply::RPL_NAMREPLY, r_params).getReplyMessage());

		chan_idx++;
	}
}
