#ifndef FATALERROR_HPP
# define FATALERROR_HPP

# include "Error.hpp"

// 프로그램을 바로 종료해야 하는 에러.
class	FatalError : public Error
{
	public:
		enum	errType
		{
			ENOERR = 0,
			ESYSERR,
			EARGC,
			EAPORT,
		};

		FatalError(enum errType t_err_no, const char* t_err_title = NULL);
		virtual	~FatalError();

		virtual std::string	what() const;
};

inline int	wrapSyscall(int syscall_ret, const char* syscall) throw(Signal, FatalError)
{
	if (g_signo == SIGINT)
		throw Signal(SIGINT);

	if (syscall_ret == -1)
		throw FatalError(FatalError::ESYSERR, syscall);

	return syscall_ret;
}

#endif
