#ifndef ERROR_HPP
# define ERROR_HPP

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

		Error(enum errType type, const char* title);
		enum errType	getErrNo(void) const;
		void	ft_perror() const;

	private:
		enum errType err_no;
		const char*	err_title;
};

#endif
