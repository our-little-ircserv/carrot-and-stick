#ifndef PARSER_HPP
# define PARSER_HPP

# include <cinttypes>

namespace	Parser
{
	uint16_t	checkArgValidity(int argc, char** argv) throw(Error);
};

#endif
