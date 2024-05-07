#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <cctype>
# include <string>
# include <vector>

// #include "Server.hpp"

namespace Command
{
	enum CmdType
	{
		PASS,
		NICK,
		USER,
		NO_SUCH_COMMAND = -1
	};

	static std::string CmdList[3] = {
		"PASS", "NICK",	"USER"
	};

	// 추후에 함수 배열 추가

	int		getCmdType(std::string& command);
	void	executeCmd(std::string cmd, std::vector< std::string >& params);
	void	CmdPass(std::vector< std::string >& params);
};

#endif