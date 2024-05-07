#include "Parser.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
int main()
{
	while (true)
	{
		struct Parser::MsgToken mt;

		char buf[1024];
		int read_size;

		read_size = read(STDIN_FILENO, buf, 1023);
		buf[read_size] = '\0';

		std::string str(buf);

		std::cout << "input: " << str << "\n";

		// 공백이 여러개인경우 다 무시한다
		// 나중에 하나의 메세지로 합칠때는 하나의 공백으로만 구분되게 한다

		size_t idx = 0;

		// extract prefix
		if (str[idx] == ':')
		{
			size_t pos = str.find(' ');
			if (pos == std::string::npos)
			{
				std::cout << "error\n";
				continue ;
			}
			mt.prefix = str.substr(idx, pos);
			idx = pos;
		}

		// skip blanks
		while (str[idx] == ' ')
		{
			idx++;
		}

		// extract command
		{
			size_t pos = str.find(' ', idx);
			mt.command = str.substr(idx, pos - idx);
			if (pos == std::string::npos)
			{
				pos = str.length() - 1;
			}
			idx = pos;
		}

		// skip blanks
		while (str[idx] == ' ')
		{
			idx++;
		}

		// extract parameteres
		while (idx < str.length() - 1)
		{
			std::string tmp;
			// check trailing
			if (str[idx] == ':')
			{
				// 공백포함 모두 복사한다
				tmp = str.substr(idx);
				mt.params.push_back(tmp);
				break ;
			}

			// check middle
			// newline 주의
			else
			{
				size_t pos = str.find(' ', idx);
				tmp = str.substr(idx, pos - idx);
				mt.params.push_back(tmp);
				if (pos == std::string::npos)
				{
					pos = str.length() - 1;
				}
				idx = pos;
			}

			// skip blanks
			while (str[idx] == ' ')
			{
				idx++;
			}
		}

		std::cout << "-----------\n";
		std::cout << "prefix : " << mt.prefix << "\n";
		std::cout << "command: " << mt.command << "\n";
		std::cout << "params :\n";

		std::vector< std::string >::iterator it = mt.params.begin();
		std::vector< std::string >::iterator ite = mt.params.end();
		for (; it != ite; it++)
		{
			if (it != mt.params.begin())
			{
				std::cout << ", ";
			}
			std::cout << *it;
		}
		std::cout << "-----------\n\n";
	}
}