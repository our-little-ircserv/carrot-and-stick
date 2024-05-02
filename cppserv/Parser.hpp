#ifndef PARSER_HPP
# define PARSER_HPP

# include <inttypes.h>
# include <exception>

namespace	Parser
{
	uint16_t	checkArgValidity(int argc, char** argv) throw(Error);
};

#endif
