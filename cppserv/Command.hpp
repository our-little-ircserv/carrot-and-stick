#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>
# include "Error.hpp"

namespace	Command
{
	struct	Data
	{
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	parameters;
	};

	struct	Join
	{
		std::vector<std::string>	channels;
		std::vector<std::string>	keys;
	};

	void	join(const std::vector<std::string>& params) throw (Error);
};

#endif
