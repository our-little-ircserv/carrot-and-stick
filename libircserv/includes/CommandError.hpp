#ifndef COMMANDERROR_HPP
# define COMMANDERROR_HPP

# include "Error.hpp"

/*
 * 해당 명령어가 존재하지 않음
 * 존재하지 않는 채널
 * 존재하지 않는 유저
 * 인자 수가 충분하지 않음
 * 해당 채널에 가입되어있지 않음
 * 해당 채널의 관리자가 아님
 */
class	CommandError : public Error
{
	public:
		enum errType
		{
			ENOERR = 0, // No error
			ENOCMD,     // No such command
			ENOCHAN,    // No such channel
			ENOUSER,    // No such user
			ENEPARM,    // Not enough parameter
			ENOTMEMB,   // Not a member
			ENOTOPER    // Not an operator
		};

		CommandError(enum errType t_err_no, const char t_err_title = NULL);
		virtual ~CommandError();

		virtual std::string	what() const;
};

#endif
