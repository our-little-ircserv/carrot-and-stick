#include "Parser.hpp"
#include "Command.hpp"
#include "Reply.hpp"

// 함수 포인터 배열을 초기화합니다.
// 추후에 Command가 추가, 삭제 되었다면 여기서 수정한다.
void Command::init()
{
	Command::cmdFunctions.push_back(Command::quit);
	Command::cmdFunctions.push_back(Command::pass);
	Command::cmdFunctions.push_back(Command::nick);
	Command::cmdFunctions.push_back(Command::user);
	Command::cmdFunctions.push_back(Command::join);
	Command::cmdFunctions.push_back(Command::topic);
	Command::cmdFunctions.push_back(Command::invite);
	Command::cmdFunctions.push_back(Command::kick);
	Command::cmdFunctions.push_back(Command::mode);
	Command::cmdFunctions.push_back(Command::privmsg);
	Command::cmdFunctions.push_back(Command::part);
	Command::cmdFunctions.push_back(Command::names);
}

int Command::getType(std::string& command)
{
	// command 인자를 대문자로 변환한 값을 담기위한 임시변수.
	std::string t_str(command);

	for (size_t i = 0; i < t_str.size(); i++)
	{
		t_str[i] = std::toupper(t_str[i]);
	}

	for (size_t i = 0; i < sizeof(Command::CmdList) / sizeof(Command::CmdList[0]); i++)
	{
		if (Command::CmdList[i] == t_str)
		{
			// command 인자를 대문자 버전으로 덮어쓴다.
			command = t_str;
			return (i);
		}
	}
	// 찾을 수 없는 명령이라면 command 인자를 원본 그대로 둔다.
	// enum UNKNOWNCOMMAND 를 반환한다.
	return (Command::UNKNOWNCOMMAND);
}

void Command::execute(IRC& server, Client& client, struct Parser::Data& data)
{
	int							cmd_type;
	std::vector< std::string >	r_params;

	// 메세지 브로드캐스팅 시 필요한, 해당 클라이언트의 prefix 부분을 저장한다.
	data.prefix = client.getPrefix();
	// 문자열 비교를 통해 함수포인터 배열의 인덱스를 탐색한다.
	cmd_type = getType(data.command);

	try
	{
		// 명령을 찾지 못한경우 ERR_UNKNOWNCOMMAND 를 throw 한다.
		if (cmd_type == Command::UNKNOWNCOMMAND)
		{
			r_params.push_back(data.command);
			throw Reply(Reply::ERR_UNKNOWNCOMMAND, r_params);
		}
		// 등록을 끝마치기전에 다른 명령어를 시도할경우 ERR_NOTREGISTERED 를 throw 한다.
		else if (client.getRegisterLevel() < Client::REGISTERED && cmd_type > Command::USER)
		{
			throw Reply(Reply::ERR_NOTREGISTERED, r_params);
		}
		// 함수포인터 배열에서 command 인자에 맞는 함수를 실행시킨다.
		Command::cmdFunctions[cmd_type](server, client, data);
	}
	// 오류 메세지를 해당 클라이언트에게 전달한다.
	catch (Reply& reply)
	{
		std::set< Client* > target_list;

		target_list.insert(&client);
		server.deliverMsg(target_list, reply.getReplyMessage(client));
	}
}

void	Command::handleClientDepartment(IRC& server, Channel* channel, Client& client)
{
	client.delChannelList(channel->getChannelName());

	channel->delMember(client);
	if (channel->getMemberCnt() == 0)
	{
		server.delChannel(channel);
	}
}
