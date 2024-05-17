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

	return data;
}
