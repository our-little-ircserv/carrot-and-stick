#include "Parser.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
int main()
{
	struct Parser::MsgToken a;

	while (true)
	{
		char buf[1024];
		int read_size;

		read_size = read(STDIN_FILENO, buf, 1023);
		buf[read_size] = '\0';

		std::cout << "input: " << buf << "\n";
	}
}