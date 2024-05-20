#include <iostream> // for debug
#include "Parser.hpp"
#include "Command.hpp"
#include "Reply.hpp"

extern std::string password;

void Command::init()
{
	Command::cmdFunctions.push_back(Command::pass);
	Command::cmdFunctions.push_back(Command::join);
	Command::cmdFunctions.push_back(Command::topic);
	Command::cmdFunctions.push_back(Command::invite);
	Command::cmdFunctions.push_back(Command::nick);
	Command::cmdFunctions.push_back(Command::kick);
	Command::cmdFunctions.push_back(Command::privmsg);
}

int Command::getType(std::string& command)
{
	for (size_t i = 0; i < command.size(); i++)
	{
		command[i] = std::toupper(command[i]);
	}

	for (int i = 0; i < sizeof(Command::CmdList) / sizeof(Command::CmdList[0]); i++)
	{
		if (Command::CmdList[i] == command)
		{
			return (i);
		}
	}
	return (-1);
}

void Command::execute(IRC& server, Client& client, struct Parser::Data& data)
{
	int							cmd_type;
	std::vector< std::string >	r_params;

	data.prefix = client.getHostname();
	cmd_type = getType(data.command);
	// remove later
	if (cmd_type == -1)
	{
		return;
	}

	try
	{
		Command::cmdFunctions[cmd_type](server, client, data);
	}
	catch (Reply& reply)
	{
		// for debug
		std::cout << reply.getReplyMessage() << std::endl;
	}
}
