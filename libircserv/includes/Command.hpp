#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <cctype>
# include <string>
# include <vector>
# include "Error.hpp"

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

	struct	Join
	{
		std::vector< std::string >	channels;
		std::vector< std::string >	keys;
	};

	struct	Privmsg
	{
		std::vector< std::string >	msg_targets;
		std::string					text_to_be_sent;
	};

	static std::string CmdList[3] = {
		"PASS", "NICK",	"USER"
	};

	// 여기선 reference 변수를 사용 할 수 없다. 어째서...?
	static std::vector< void (*)(std::vector< std::string >) > cmdFunctions;

	// 추후에 함수 배열 추가

	void	init();
	int		getType(std::string& command);
	void	execute(std::string cmd, std::vector< std::string >& params);

	// command implementations
	void	pass(const std::vector< std::string > params) throw(Error);
	void	join(const std::vector< std::string > params) throw(Error);
};

#endif
