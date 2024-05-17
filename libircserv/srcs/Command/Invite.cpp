#include "Parser.hpp"
#include "Command.hpp"

struct Command::Invite	Parser::invite(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Invite		data;
	std::vector< std::string >	r_params;

	if (params.size() < 2)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.nickname = params[0];
	data.channel = params[1];

	return data;
}

void	Command::invite(IRC& server, Client& client, const std::vector< std::string >& params) throw (Error)
{
	struct Command::Invite	data;
	Channel* channel;
	Client* target_client;

	data = Parser::invite(params);

	// 초대하는 자가 해당 채널에 접속중인지 확인 - 해당 채널이 실존하는지는 검사하지 않음
	// class Client 쪽에 class Channel 을 목록으로 들고있어야 하나 싶지만
	// 우선 실존하지 않는 채널이면 무조건 해당 채널에 접속중이지 않다는 전제로 구현
	// ERR_NOTONCHANNEL
	channel = server.searchChannel(data.channel);
	if (channel == NULL || channel->isMember(client) == false)
	{
		throw(Error::EWRPARM);
	}

	// 초대받는 자가 실존하는 유저인지 검사
	// ERR_NOSUCHNICK
	target_client = server.searchClient(data.nickname);
	if (target_client == NULL)
	{
		throw(Error::EWRPARM);
	}

	// 초대받는 자가 해당 채널에 이미 존재하는지 확인
	// ERR_USERONCHANNEL
	if (channel->isMember(*target_client) == true)
	{
		throw(Error::EWRPARM);
	}
	// +i 플래그가 활성화되었다면 초대하는 자가 채널 관리자인지 확인
	// ERR_CHANOPRIVSNEEDED
	else if (channel->checkModeSet('i') == true && channel->isOperator(client) == false)
	{
		throw(Error::EWRPARM);
	}
	// 이미 초대받은 상태인지 중복확인 후 초대목록에 추가
	// RPL_INVITING
	else if (channel->isInvited(*target_client) == false)
	{
		channel->addInvited(*target_client);
	}
}
