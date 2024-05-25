#include "Parser.hpp"
#include "Command.hpp"
#include "Assert.hpp"

struct Command::Privmsg	Parser::privmsg(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Privmsg		data;
	std::string					t_msg_targets = params[0];
	std::string					t_target;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NORECIPIENT, r_params);
	}
	else if (params.size() < 2)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NOTEXTTOSEND, r_params);
	}

	size_t	i = 0;
	size_t	offset;
	while (i < t_msg_targets.size())
	{
		offset = t_msg_targets.find_first_of(',', i);
		if (offset >= t_msg_targets.size())
		{
			offset = t_msg_targets.size();
		}
		offset -= i;

		t_target = t_msg_targets.substr(i, offset);
		data.msg_targets.push_back(t_target);

		i += offset + 1;
		if (data.msg_targets.size() == 5)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(t_msg_targets);
			throw Reply(Reply::ERR_TOOMANYTARGETS, r_params);
		}
	}

	data.text_to_be_sent = params[1];;

	return data;
}

void	Command::privmsg(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Privmsg		p_data;
	std::vector< std::string >	r_params;

	p_data = Parser::privmsg(client, data.parameters);

	for (size_t i = 0; i < p_data.msg_targets.size(); i++)
	{
		std::set< Client* >	target_list;
		Client*				t_client;
		Channel*			t_channel;

		try
		{
			t_channel = server.searchChannel(p_data.msg_targets[i]);
			if (t_channel == NULL)
			{
				t_client = server.searchClient(p_data.msg_targets[i]);
				if (t_client == NULL)
				{
					r_params.push_back(client.getNickname());
					r_params.push_back(p_data.msg_targets[i]);
					throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
				}
				target_list.insert(t_client);
			}
			else
			{
				Assert(t_channel->getMemberCnt() != 0);
				std::set< Client* >	t_member_set = t_channel->getMemberSet();

				target_list.insert(t_member_set.begin(), t_member_set.end());
			}

			std::set< Client* >::iterator it = target_list.find(&client);
			if (it != target_list.end())
			{
				target_list.erase(it);
			}

			r_params.push_back(data.prefix);
			r_params.push_back(data.command);
			r_params.push_back(p_data.msg_targets[i]);
			r_params.push_back(p_data.text_to_be_sent);
			r_params.push_back("\r\n");

			server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			r_params.clear();
		}
		catch(Reply& e)
		{
			// ErrReply to host client
			target_list.insert(&client);
			server.deliverMsg(target_list, e.getReplyMessage());
		}
	}
}
