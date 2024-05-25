#ifndef ASSERT_HPP
# define ASSERT_HPP

# include <cstdlib>
# include <cstdio>
# include <cstdbool>

# ifdef DEBUG
#  define Assert(exp) if (exp == false) \
{ \
	std::fprintf(stderr, "Assertion failed:\n  \033[31m%s\n  \033[0mline: %d\n  function: %s\n  file: %s\n", #exp, __LINE__, __FUNCTION__, __FILE__); \
	std::perror("\033[31m  errno"); \
	std::fprintf(stderr, "\033[0m"); \
	abort(); \
}
# else
#  define Assert(exp) ((void)0)
# endif

#endif
