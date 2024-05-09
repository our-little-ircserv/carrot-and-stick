#include <iostream>

// #include "Server.hpp"
// #include "Channel.hpp"
#include "Client.hpp"
// #include "Parser.hpp"

int main()
{
	struct sockaddr_in ad;
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = inet_addr("127.0.0.1");
	ad.sin_port = htons(80);

	Client a(1, ad);
	a.setNickname("nick");
	a.setUsername("user");
	a.setRealname("real");

	std::cout << "prefix: " << a.getPrefix() << "\n";
}