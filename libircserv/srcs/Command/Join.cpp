#include "Parser.hpp"
#include "Command.hpp"

struct Command::Join	Parser::join(const std::vector< std::string > params) throw(Error)
{
	struct Command::Join	data;
	const std::string&			t_channels = params[0];
	const std::string&			t_keys = params[1];

	size_t	i = 0;
	size_t	offset;
	std::string	channel_name;
	while (i < t_channels.size())
	{
		offset = t_channels.find_first_of(',', i);
		if (offset > t_channels.size())
		{
			offset = t_channels.size();
		}
		offset -= i;

		if (offset > 50)
		{
			offset = 50;
		}

		channel_name = t_channels.substr(i, offset);
		if (Parser::isValidChannelName(channel_name) == false)
		{
			throw Error(Error::EWRPARM, channel_name.c_str());
		}

		i += offset + 1;
	}

	i = 0;
	std::string	key_value;
	while (t_keys[i] != '\0')
	{
		offset = t_keys.find_first_of(',', i) - i;
		if (offset >= t_keys.size())
		{
			offset = t_keys.size() - i;
		}
		key_value = t_keys.substr(i, offset);
		data.keys.push_back(key_value);
		i += offset;
	}

	return data;
}

void	Command::join(const std::vector< std::string > params) throw (Error)
{
	struct Command::Join data = Parser::join(params);

	// ...
}
