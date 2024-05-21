#include "Parser.hpp"
#include "Command.hpp"

struct Command::Part	Parser::part(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Part		data;
	std::vector< std::string >  r_params;

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

    if (params.size() == 1)
    {
        return data;
    }

    data.comment = params[1];

	return data;
}

void	Command::part(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	//
}
