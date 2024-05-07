#ifndef PARSER_HPP
# define PARSER_HPP

#include <string>
#include <vector>

namespace Parser
{
	struct MsgToken
	{
		std::string					prefix;
		std::string					command;
		std::vector< std::string >	params;
	};

	// fucntions...
}

#endif