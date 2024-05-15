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

		virtual	~FatalError();

		FatalError(enum errType _err_no, const char* _err_title = NULL);
		virtual std::string	what() const;
};

inline int	wrapSyscall(int syscall_ret, const char* syscall) throw(Signal, Error)
{
	if (g_signo == SIGINT)
		throw Signal(SIGINT);

	if (syscall_ret == -1)
		throw Error(Error::ESYSERR, syscall);

	return syscall_ret;
}

#endif
