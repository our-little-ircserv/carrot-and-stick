#include "Parser.hpp"
#include "Command.hpp"

struct Command::Mode	Parser::mode(const std::vector< std::string >& params) throw(Error)
{
	struct Command::Mode	data;

	if (params.size() < 1)
	{
		throw Error(Error::ENEPARM, NULL);
	}

	data.channel = params[0];

	size_t	i = 1;
	while (i < params.size())
	{
		struct Command::ModeWithParams	t_mode_with_params;

		size_t new_modes = 0;
		if (Parser::isMode(params[i][0]) == true)
		{
			new_modes = Parser::insertModes(data, params[i], params[i][0]);
			++i;
		}

		if (i < params.size() && Parser::isMode(params[i][0]) == false)
		{
			i = insertModeParameters(data, params, new_modes, i);
		}
	}

	return data;
}
//
//int main()
//{
//	std::vector< std::string > params;
//	params.push_back("#channel");
//	params.push_back("+iok");
////	params.push_back("+i");
////	params.push_back("+o");
////	params.push_back("+k");
//	params.push_back("operator");
//	params.push_back("key");
//	params.push_back("-io");
//	params.push_back("operator");
//
//	struct Command::Mode mode = Parser::mode(params);
//
//	std::cout << Parser::concat_string_vector(params) << std::endl;
//	std::cout << "======================" << std::endl;
//	std::cout << mode.channel << std::endl;
//	for (std::vector< struct Command::ModeWithParams >::iterator it = mode.modes.begin(); it != mode.modes.end(); it++)
//	{
//		std::cout << "======================" << std::endl;
//		std::cout << it->type << std::endl;
//		std::cout << it->mode << std::endl;
//		std::cout << it->mode_param << std::endl;
//	}
//}
