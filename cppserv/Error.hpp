#ifndef ERROR_HPP
# define ERROR_HPP

# include <signal.h>
# include "Signal.hpp"

extern volatile sig_atomic_t g_signo;

// error 종류가 많아지지 않는다면.
// 추후에 Error를 상속받는 파생 errors 정의하기.
// errno 아쉽지만 버려~~
class	Error
{
	public:
		enum	errType
		{
			ENOERR = 0,
			ESYSERR,
			EARGC,
			EAPORT,
			EAPASS
		};

		Error(enum errType _err_no, const char* _err_title);

		enum errType	getErrNo(void) const;
		void			ftPerror() const;

	private:
		enum errType	m_err_no;
		const char*		m_err_title;
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
