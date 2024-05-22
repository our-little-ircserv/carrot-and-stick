#include "IRC.hpp"
#include "Parser.hpp"
#include "Command.hpp"

// Don't need this function in 'QUIT' command
// void	Parser::quit(const std::vector< std::string > params)
// {
// 	//
// }

void	Command::quit(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	std::vector< std::string >	r_params;
    std::vector< std::string >  chan_list;
    std::set< Client* >         target_list;

    r_params.push_back(client.getNickname());
    r_params.push_back(data.command);
    if (data.parameters.empty() == false)
    {
        r_params.push_back(data.parameters[0]);
    }
    r_params.push_back("\r\n");

    // 자신을 제외하고 모두에게 quit 메세지 브로드캐스트
    chan_list.insert(chan_list.end(), client.getChannelList().begin(), client.getChannelList().end());
    target_list = server.getTargetSet(chan_list);
    target_list.erase(target_list.find(&client));

    server.deliverMsg(target_list, Parser::concat_string_vector(r_params));

    // 본인에게는 ERROR 메세지 전달
    target_list.clear();
    r_params.clear();
    target_list.insert(&client);
    r_params.push_back("ERROR");
    if (data.parameters.empty() == false)
    {
        r_params.push_back(data.parameters[0]);
    }
    r_params.push_back("\r\n");

    server.deliverMsg(target_list, Parser::concat_string_vector(r_params));

    // update channel's member_list
    for (size_t i = 0; i < chan_list.size(); i++)
    {
        Channel* channel = server.searchChannel(chan_list[i]);

        // if (channel == NULL) || channel->isMember(client) == false
        // {
        //     ASSERT
        // }

        if (channel->isOperator(client) == true)
        {
            channel->delOperator(client);
        }
        channel->delMember(client);
    }

    server.delClient(client.getSocketFd());
}
