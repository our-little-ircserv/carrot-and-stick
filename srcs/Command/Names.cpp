#include "Parser.hpp"
#include "Command.hpp"

std::vector< std::string >	Parser::names(const std::vector< std::string >& params) throw(Reply)
{
	std::vector< std::string >	data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back("NAMES");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	size_t				i = 0;
	size_t				offset;
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
		data.push_back(channel_name);
		i += offset + 1;
	}

	return data;
}

void	Command::names(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	std::vector< std::string >	channels = Parser::names(data.parameters);
	size_t						channels_size = channels.size();
	size_t						chan_idx = 0;
	std::vector< std::string >	r_params;
	std::set< Client* >			target_list;


	// 채널의 존재여부나 이름의 유효성은 검사하지 않는다.
	target_list.insert(&client);
	while (chan_idx < channels_size)
	{
		r_params.clear();
		try
		{
			Channel*	channel = server.searchChannel(channels[chan_idx]);

			// 채널이 실존하고 해당 채널에 접속해 있는 경우만 목록을 표시한다.
			if ((channel == NULL) == false && channel->isMember(client) == true)
			{
				r_params.push_back("=");
				r_params.push_back(channels[chan_idx]);

				std::map<Client*, bool>::iterator it = channel->getMemberEnd();
				std::map<Client*, bool>::iterator ite = channel->getMemberBegin();
				std::vector< std::string > nick_list;
				std::string t_nick;

				// 가장 최근에 접속한 클라이언트부터 목록에 추가한다.
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
			server.deliverMsg(target_list, e.getReplyMessage(client));
		}

		// 목록의 끝을 나타내는 메세지를 전달한다.
		r_params.clear();
		r_params.push_back(channels[chan_idx]);
		r_params.push_back(":End of NAMES list");

		server.deliverMsg(target_list, Reply(Reply::RPL_ENDOFNAMES, r_params).getReplyMessage(client));

		chan_idx++;
	}
}
