#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>

namespace	Command
{
	enum	Type
	{
		PASS,
		JOIN
	};

	struct	Data
	{
		// ?prefix
//		enum Type	command;
		std::string	command;
		void*		parameters;
	};

	int	executeCommand(struct Command::Data data);
	int	pass(struct Command::Data* data);
};

#endif
