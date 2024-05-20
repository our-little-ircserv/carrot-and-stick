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

void	Command::invite(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Invite		p_data;
	std::vector< std::string >	r_params;
	Channel*					channel;
	Client*						target_client;

	p_data = Parser::invite(client, data.parameters);

	// 초대하는 자가 해당 채널에 접속중인지 확인 - 해당 채널이 실존하는지는 검사하지 않음
	// class Client 쪽에 class Channel 을 목록으로 들고있어야 하나 싶지만
	// 우선 실존하지 않는 채널이면 무조건 해당 채널에 접속중이지 않다는 전제로 구현
	// ERR_NOTONCHANNEL
	channel = server.searchChannel(p_data.channel);
	if (channel == NULL || channel->isMember(client) == false)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
	}

	// 초대받는 자가 실존하는 유저인지 검사
	// ERR_NOSUCHNICK
	target_client = server.searchClient(p_data.nickname);
	if (target_client == NULL)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_NOSUCHNICK, r_params);
	}

	// 초대받는 자가 해당 채널에 이미 존재하는지 확인
	// ERR_USERONCHANNEL
	if (channel->isMember(*target_client) == true)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(target_client->getNickname());
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_USERONCHANNEL, r_params);
	}
	// +i 플래그가 활성화되었다면 초대하는 자가 채널 관리자인지 확인
	// ERR_CHANOPRIVSNEEDED
	else if (channel->checkModeSet('i') == true && channel->isOperator(client) == false)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
	}
	// 이미 초대받은 상태인지 중복확인 후 초대목록에 추가
	// RPL_INVITING
	else if (channel->isInvited(*target_client) == false)
	{
		channel->addInvited(*target_client);
		r_params.push_back(p_data.channel);
		r_params.push_back(client.getNickname());
		// Reply(Reply::RPL_INVITING, r_params);
	}
}
