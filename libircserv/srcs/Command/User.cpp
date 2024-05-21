#include "Parser.hpp"
#include "Command.hpp"

struct Command::User	Parser::user(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct Command::User		data;
	std::vector< std::string >	r_params;

	if (params.size() < 4)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back("USER");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.username = params[0];
	data.real_name = params[3];

	return data;
};

void	Command::user(IRC& server, Client& client, const struct Parser::Data& data) throw (Reply)
{
	struct Command::User		p_data = Parser::user(client, data.parameters);
	std::vector< std::string >	r_params;

	if (client.getRegisterLevel() == Client::NONE)
	{
		return ;
	}
	else if (client.getRegisterLevel() == Client::REGISTERED)
	{
		r_params.push_back(client.getNickname());
		throw Reply(Reply::ERR_ALREADYREGISTRED, r_params);
	}

	client.setUsername(p_data.username);
	client.setRealname(p_data.real_name);
	
	if ((client.getNickname() == "*") == false)
	{
		client.setRegisterLevel(Client::REGISTERED);
		// 환영합니다
		std::set< Client* > target_list;
		target_list.insert(&client);
		r_params.push_back(client.getNickname());

		{
			Reply rp(Reply::RPL_WELCOME, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_YOURHOST, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_CREATED, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
		{
			Reply rp(Reply::RPL_MYINFO, r_params);
			server.deliverMsg(target_list, rp.getReplyMessage());
		}
	}
}
