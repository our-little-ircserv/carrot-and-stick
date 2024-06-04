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
//
//int Command::getType(std::string& command)
//{
//	// command 인자를 대문자로 변환한 값을 담기위한 임시변수.
//	std::string t_str(command);
//	size_t	t_str_size = t_str.size();
//
//	for (size_t i = 0; i < t_str_size; i++)
//	{
//		t_str[i] = std::toupper(t_str[i]);
//	}
//
//	size_t	cmdlist_size = sizeof(Command::CmdList) / sizeof(Command::CmdList[0]);
//	for (size_t i = 0; i < cmdlist_size; i++)
//	{
//		if (Command::CmdList[i] == t_str)
//		{
//			// command 인자를 대문자 버전으로 덮어쓴다.
//			command = t_str;
//			return (i);
//		}
//	}
//	// 찾을 수 없는 명령이라면 command 인자를 원본 그대로 둔다.
//	// enum UNKNOWNCOMMAND 를 반환한다.
//	return Command::UNKNOWNCOMMAND;
//}

// execute command with trimmed data
void Command::execute(IRC& server, Client& client, struct Parser::Data& data)
{
	int							cmd_type;
	std::vector< std::string >	r_params;

	// 메세지 브로드캐스팅 시 필요한, 해당 클라이언트의 prefix 부분을 저장한다.
	data.prefix = client.getPrefix();

	// command를 복사해서 대문자로 바꾼다.
	std::string t_str(data.command);
	size_t	t_str_size = t_str.size();

	for (size_t i = 0; i < t_str_size; i++)
	{
		t_str[i] = std::toupper(t_str[i]);
	}

	// 대문자로 바꾼 command를 cmdlist와 비교해서 찾는다.
	size_t	cmdlist_size = sizeof(Command::CmdList) / sizeof(Command::CmdList[0]);
	size_t	i = 0;
	for ( ; i < cmdlist_size; i++)
	{
		if (Command::CmdList[i] == t_str)
		{
			// command 인자를 대문자 버전으로 덮어쓴다.
			data.command = t_str;
			break;
		}
	}
	cmd_type = i < cmdlist_size ? i : Command::UNKNOWNCOMMAND;

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
