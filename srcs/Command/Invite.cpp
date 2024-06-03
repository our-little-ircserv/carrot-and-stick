#include "Parser.hpp"
#include "Command.hpp"

struct Command::Invite	Parser::invite(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Invite		data;
	std::vector< std::string >	r_params;

	if (params.size() < 2)
	{
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

	p_data = Parser::invite(data.parameters);

	// 채널의 존재여부를 따로 확인하지 않는다.
	// 채널이 존재하지 않거나 초대자가 해당 채널에 존재하지 않을 경우
	// Reply를 throw 한다.
	channel = server.searchChannel(p_data.channel);
	if (channel == NULL || channel->isMember(client) == false)
	{
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
	}

	// 초대받는 클라이언트가 실존하는 유저인지 검사한다.
	target_client = server.searchClient(p_data.nickname);
	if (target_client == NULL)
	{
		throw Reply(Reply::ERR_NOSUCHNICK, r_params);
	}

	// 초대받는 클라이언트가 해당 채널에 이미 존재하는지 확인한다.
	if (channel->isMember(*target_client) == true)
	{
		r_params.push_back(target_client->getNickname());
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_USERONCHANNEL, r_params);
	}
	// +i 플래그가 활성화되었다면 초대하는 자가 채널 관리자인지 확인한다.
	else if (channel->checkModeSet('i') == true && channel->isOperator(client) == false)
	{
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
	}

	// 이미 초대받은 상태인지 중복확인 후 초대목록에 추가한다.
	if (channel->checkModeSet('i') == true)
	{
		channel->addInvited(*target_client);
	}

	// RPL_INVITING 메세지를 초대자에게 전송한다.
	r_params.push_back(p_data.channel);
	r_params.push_back(p_data.nickname);

	server.deliverMsg(&client, Reply(Reply::RPL_INVITING, r_params).getReplyMessage(client));

	// 초대받는 클라이언트에게 INVITE 메세지를 전송한다.
	r_params.insert(r_params.begin(), data.command);
	r_params.insert(r_params.begin(), data.prefix);
	r_params[2] = p_data.nickname;
	r_params[3] = p_data.channel;
	r_params.push_back("\r\n");

	server.deliverMsg(target_client, Parser::concat_string_vector(r_params));
}
