#include <iostream>
#include "Parser.hpp"
#include "Command.hpp"

extern std::string password;

void Command::init()
{
	Command::cmdFunctions.push_back(Command::pass);
	Command::cmdFunctions.push_back(Command::join);
	Command::cmdFunctions.push_back(Command::topic);
	Command::cmdFunctions.push_back(Command::invite);
	Command::cmdFunctions.push_back(Command::nick);
}

int Command::getType(std::string& command)
{
	for (size_t i = 0; i < command.size(); i++)
	{
		command[i] = std::toupper(command[i]);
	}

	for (int i = 0; i < Command::CmdList->size(); i++)
	{
		if (Command::CmdList[i] == command)
			return (i);
	}
	return (-1);
}

void Command::execute(IRC& server, Client& client, struct Parser::Data& data)
{
	int cmd_type;

	cmd_type = getType(data.command);

	Command::cmdFunctions[cmd_type](server, client, data.parameters);
}

