#ifndef ERROR_HPP
# define ERROR_HPP

# include <signal.h>
# include "Signal.hpp"

extern volatile sig_atomic_t g_signo;

class	Error
{
	public:
		Error(unsigned int t_err_no, const char* t_err_title = NULL);
		virtual ~Error();

		int		getErrNo(void) const;
		void	ftPerror() const;

		virtual std::string	what() const = 0;

	protected:
		int			_err_no;
		const char*	_err_title;
};
//
//inline int	wrapSyscall(int syscall_ret, const char* syscall) throw(Signal, Error)
//{
//	if (g_signo == SIGINT)
//		throw Signal(SIGINT);
//
//	if (syscall_ret == -1)
//		throw Error(Error::ESYSERR, syscall);
//
//	return syscall_ret;
//}

#endif
