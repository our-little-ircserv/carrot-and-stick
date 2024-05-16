#include "CommandError.hpp"

CommandError::CommandError(enum errType t_err_no, const char* t_err_title) : Error(static_cast< int >(t_err_no), t_err_title)
{
}

CommandError::~CommandError()
{
}

std::string	CommandError::what() const
{
	std::string	error_message;

	if (_err_title != NULL)
	{
		error_message = _err_title + ": ";
	}

	switch (_err_no)
	{
		case ENOCMD:
			//
			break;
		case ENOCHAN:
			//
			break;
		case ENOUSER:
			//
			break;
		case ENEPARM:
			//
			break;
		case ENOTMEMB:
			//
			break;
		case ENOTOPER:
			//
			break;
		default:
			break;
	}

	return error_message;
}
