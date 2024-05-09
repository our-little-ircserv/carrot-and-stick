#include <iostream>
#include "Command.hpp"

extern std::string password;

void Command::init()
{
	Command::cmdFunctions.push_back(Command::pass);
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

void Command::execute(std::string cmd, std::vector< std::string >& params)
{
	int cmd_type;

	cmd_type = getType(cmd);

	Command::cmdFunctions[cmd_type](params);
}

void Command::pass(std::vector< std::string > params) throw(Error)
{
	std::cout << "PASS command Executed!\n";

	if (password == params[0])
	{
		std::cout << "correct password!\n";
	}
	else
	{
		std::cout << "wrong password!\n";
	}
}

