#include "Parser.hpp"
#include "Command.hpp"

struct Command::Privmsg	Parser::privmsg(const std::vector< std::string > params)
{
	struct Command::Privmsg	data;
	std::string				t_msg_targets = params[0];
	std::string				t_target;

	if (params.size() < 2)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENEPARM, concat_params.c_str());
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
	}

	data.text_to_be_sent = params[1];;

	return data;
}
