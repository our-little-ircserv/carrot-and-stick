#include "Parser.hpp"
#include "Command.hpp"

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
