#include "Parser.hpp"
#include "Command.hpp"

struct Command::User	Parser::user(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::User		data;
	std::vector< std::string >	r_params;

	if (params.size() < 4)
	{
		r_params.push_back("USER");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.username = params[0];
	data.real_name = params[3];

	return data;
};

void	Command::user(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	struct Command::User		p_data = Parser::user(data.parameters);
	std::vector< std::string >	r_params;

	// 서버의 비밀번호를 제대로 입력하지 않았다면 무시합니다.
	if (client.getRegisterLevel() == Client::NONE)
	{
		return ;
	}
	// 이미 등록을 마친 상태라면 무시합니다.
	else if (client.getRegisterLevel() == Client::REGISTERED)
	{
		throw Reply(Reply::ERR_ALREADYREGISTRED, r_params);
	}

	// 매개변수 중 username 과 realname 만 설정합니다
	client.setUsername(p_data.username);
	client.setRealname(p_data.real_name);

	// 등록이 완료된 직후
	// Reply 001 ~ 004 까지 해당 클라이언트에게 전달합니다.
	if ((client.getNickname() == "*") == false)
	{
		// 등록 단계를 REGISTERED로 설정합니다.
		client.setRegisterLevel(Client::REGISTERED);
		std::set< Client* > target_list;
		target_list.insert(&client);

		{
			Reply rp(Reply::RPL_WELCOME, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			Reply rp(Reply::RPL_YOURHOST, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			r_params.push_back(server.getStartTime());
			Reply rp(Reply::RPL_CREATED, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
		{
			Reply rp(Reply::RPL_MYINFO, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage(client));
		}
	}
}
