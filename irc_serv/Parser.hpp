#ifndef PARSER_HPP
# define PARSER_HPP

#include <string>
#include <vector>

#include "Command.hpp"

namespace Parser
{
	/*
		Prefix는 메시지가 어디에서 왔는지를 나타냅니다.
		이는 메시지가 사용자로부터 온 것인지, 아니면 서버에서 발생한 것인지를 구분하는 데 사용됩니다.

		서버에서 클라이언트로:
		서버가 클라이언트에 메시지를 보낼 때, 접두어는 일반적으로 서버의 호스트명이 됩니다.
		이는 메시지가 네트워크의 어느 서버에서 왔는지를 클라이언트가 알 수 있게 해줍니다.

		클라이언트에서 서버로:
		클라이언트가 서버로 메시지를 보낼 때 접두어를 사용할 필요는 없습니다.
		하지만 클라이언트가 다른 사용자에게 메시지를 보내는 경우, 자신의 닉네임이나 호스트명을 접두어로 포함할 수 있습니다.

		- 클라이언트에서 이상한 접두사를 붙여도 서버에서 보정해줌...?
		- 추후에 토큰들을 하나의 메세지로 만들어내는 함수가 필요할것같음
	*/

	struct MsgToken
	{
		std::string					prefix;
		std::string					command;
		std::vector< std::string >	params;
	};

	// fucntions...
	void			checkArgValid();
	struct MsgToken	parseMessage(std::string msg);
}

#endif