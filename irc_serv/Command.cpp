#include "Command.hpp"

#include <iostream>

extern std::string password;

int Command::getCmdType(std::string& command)
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

void Command::executeCmd(std::string cmd, std::vector< std::string >& params)
{
	int cmd_type;

	cmd_type = getCmdType(cmd);
	switch (cmd_type)
	{
		case Command::PASS :
			Command::CmdPass(params);
			break;
		
		default:
			std::cout << "wrong cmd\n";
			break;
	}
}

void Command::CmdPass(std::vector< std::string >& params)
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