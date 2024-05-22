#include <iostream> // for debug
#include "Parser.hpp"
#include "Command.hpp"
#include "Reply.hpp"

extern std::string password;

void Command::init()
{
	Command::cmdFunctions.push_back(Command::pass);
	Command::cmdFunctions.push_back(Command::nick);
	Command::cmdFunctions.push_back(Command::user);
	Command::cmdFunctions.push_back(Command::join);
	Command::cmdFunctions.push_back(Command::topic);
	Command::cmdFunctions.push_back(Command::invite);
	Command::cmdFunctions.push_back(Command::kick);
	Command::cmdFunctions.push_back(Command::mode);
	Command::cmdFunctions.push_back(Command::privmsg);
	Command::cmdFunctions.push_back(Command::part);
}

int Command::getType(std::string& command)
{
	std::string t_str(command);

	for (size_t i = 0; i < t_str.size(); i++)
	{
		t_str[i] = std::toupper(t_str[i]);
	}

	for (int i = 0; i < sizeof(Command::CmdList) / sizeof(Command::CmdList[0]); i++)
	{
		if (Command::CmdList[i] == t_str)
		{
			command = t_str;
			return (i);
		}
	}
	return (Command::UNKNOWNCOMMAND);
}

void Command::execute(IRC& server, Client& client, struct Parser::Data& data)
{
	int							cmd_type;
	std::vector< std::string >	r_params;

	data.prefix = client.getPrefix();
	cmd_type = getType(data.command);

	try
	{
		// command not found
		if (cmd_type == Command::UNKNOWNCOMMAND)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(data.command);
			throw Reply(Reply::ERR_UNKNOWNCOMMAND, r_params);
		}
		else if (client.getRegisterLevel() < Client::REGISTERED && cmd_type > Command::USER)
		{
			r_params.push_back(client.getNickname());
			throw Reply(Reply::ERR_NOTREGISTERED, r_params);
		}
		Command::cmdFunctions[cmd_type](server, client, data);
	}
	catch (Reply& reply)
	{
		// for debug
		std::cout << reply.getReplyMessage() << std::endl;
	}
}
