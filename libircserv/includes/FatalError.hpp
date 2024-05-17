#ifndef FATALERROR_HPP
# define FATALERROR_HPP

extern volatile sig_atomic_t g_signo;

// 프로그램을 바로 종료해야 하는 에러.
class	FatalError
{
	public:
		enum	ErrType
		{
			ENOERR = 0,
			ESYSERR,
			EARGC,
			EAPORT
		};

		FatalError(enum ErrType t_err_no, const char* t_err_title = NULL);
		~FatalError();

		enum ErrType	getErrNo() const;
		void			ftPerror() const;

	private:
		enum ErrType	_err_no;
		const char*		_err_title;
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
