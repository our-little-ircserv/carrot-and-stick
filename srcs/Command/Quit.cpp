#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"
#include "Assert.hpp"

// Don't need this function in 'QUIT' command
// void	Parser::quit(const std::vector< std::string > params)
// {
// 	//
// }

void	Command::quit(IRC& server, Client& client, const struct Parser::Data& data) throw(enum Client::REGISTER_LEVEL)
{
	std::vector< std::string >	r_params;
	std::vector< std::string >  chan_list;
	std::set< Client* >         target_list;
	std::string					t_str;

	r_params.push_back(data.prefix);
	r_params.push_back(data.command);
	if (data.parameters.empty() == false)
	{
		t_str = data.parameters[0];
	}
	else
	{
		t_str = client.getNickname();
	}
	if (t_str[0] != ':')
	{
		t_str.insert(0, ":");
	}
	r_params.push_back(t_str);
	r_params.push_back("\r\n");

	// quit 메세지를 브로드캐스트합니다.
	chan_list.insert(chan_list.end(), client.getChannelList().begin(), client.getChannelList().end());
	if (chan_list.empty() == false)
	{
		target_list = server.getTargetSet(chan_list);
		std::set< Client* >::iterator it = target_list.find(&client);
		Assert(it != target_list.end());

		// 자기자신을 제외합니다.
		target_list.erase(it);

		if (target_list.empty() == false)
		{
			server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			target_list.clear();
		}
	}

	// 본인에게는 ERROR 메세지를 전달합니다.
	r_params.clear();
	target_list.insert(&client);
	r_params.push_back("ERROR");
	if (data.parameters.empty() == false)
	{
		t_str = data.parameters[0];
	}
	else
	{
		t_str = "Closing connection";
	}
	if (t_str[0] != ':')
	{
		t_str.insert(0, ":");
	}
	r_params.push_back(t_str);
	r_params.push_back("\r\n");

	server.deliverMsg(target_list, Parser::concat_string_vector(r_params));

	throw Client::LEFT;
}
