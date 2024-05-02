#ifndef PARSER_HPP
# define PARSER_HPP

# include <exception>

namespace	Parser
{
	void	checkArgValidity(int argc, char** argv) throw(Error);
};

#endif
