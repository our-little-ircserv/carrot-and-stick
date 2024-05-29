#include "Parser.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Assert.hpp"

struct Command::Mode	Parser::mode(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Mode		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back("MODE");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.channel = params[0];

	size_t	i = 1;
	while (i < params.size())
	{
		struct Command::ModeWithParams	t_mode_with_params;

		size_t new_modes = 0;
		if (Parser::isMode(params[i][0]) == true)
		{
			new_modes = Parser::insertModes(data, params[i], params[i][0]);
			++i;
		}

		if (i < params.size() && Parser::isMode(params[i][0]) == false)
		{
			i = Parser::insertModeParameters(data, params, new_modes, i);
		}
	}

	return data;
}

// oitkl
void	Command::mode(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Mode		p_data = Parser::mode(data.parameters);
	std::vector< std::string >	r_params;

	Channel*	channel = server.searchChannel(p_data.channel);
	if (channel == NULL)
	{
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
	}
	else if (p_data.modes.empty() == true)
	{
		r_params.push_back(p_data.channel);
		r_params.push_back(channel->getCurrentMode());
		r_params.push_back(channel->getCurrentModeParam(channel->isMember(client)));
		throw Reply(Reply::RPL_CHANNELMODEIS, r_params);
	}
	else if (channel->isMember(client) == false || channel->isOperator(client) == false)
	{
		r_params.push_back(p_data.channel);

		throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
	}

	// only (member && operator) reaches here
	std::string	valid_param_del;
	std::string	valid_param_add;

	std::vector< struct Command::ModeWithParams >::iterator it = p_data.modes.begin();
	std::vector< struct Command::ModeWithParams >::iterator ite = p_data.modes.end();
	for (; it != ite; it++)
	{
		try
		{
			channel->setMode(*it);
			if (it->type == Command::DEL)
			{
				if (valid_param_del.empty() == true)
				{
					valid_param_del = "-";
				}

				valid_param_del += it->mode;
			}
			else
			{
				if (valid_param_add.empty() == true)
				{
					valid_param_add = "+";
				}

				valid_param_add += it->mode;
			}
		}
		catch (const Reply& e)
		{
			std::set< Client* > target_list;

			target_list.insert(&client);

			server.deliverMsg(target_list, e.getReplyMessage(client));
		}
	}

	{
		std::set< Client* > target_list = channel->getMemberSet();

		r_params.push_back(data.prefix);
		r_params.push_back(data.command);
		Assert(data.parameters.empty() == false);
		r_params.push_back(data.parameters.front());
		r_params.push_back(valid_param_add);
		r_params.push_back(valid_param_del);
//		r_params.insert(r_params.end(), data.parameters.begin(), data.parameters.end());
		r_params.push_back("\r\n");

		server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
	}
}
