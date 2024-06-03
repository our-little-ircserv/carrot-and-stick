#include "Parser.hpp"
#include "Command.hpp"

struct	Command::Topic	Parser::topic(const std::vector< std::string >& params) throw(Reply)
{
	struct	Command::Topic		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back("TOPIC");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.channel = params[0];
	if (params.size() > 1)
	{
		data.topic = params[1];
	}

	return data;
}

void	Command::topic(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Topic		p_data;
	std::vector< std::string >	r_params;
	Channel*					channel;

	try
	{
		p_data = Parser::topic(data.parameters);
		channel = server.searchChannel(p_data.channel);

		// 채널의 존재여부를 따로 확인하지 않는다.
		// 채널이 존재하지 않거나 클라이언트가 해당 채널에 존재하지 않을 경우
		// Reply 를 throw 한다.
		if (channel == NULL || channel->isMember(client) == false)
		{
			r_params.push_back(p_data.channel);
			throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
		}

		// 매개변수가 하나일경우 해당 채널의 토픽만 조회한다.
		if (p_data.topic.empty() == true)
		{
			std::string t_topic;

			t_topic = channel->getTopic();

			// 설정되어있는 토픽이 없으면 RPL_NOTOPIC 를 throw 한다.
			if (t_topic.size() == 0)
			{
				r_params.push_back(p_data.channel);
				throw Reply(Reply::RPL_NOTOPIC, r_params);
			}
			// 해당 채널의 토픽을 RPL_TOPIC 로 throw 하여 클라이언트에게 전달한다.
			else
			{
				r_params.push_back(p_data.channel);
				r_params.push_back(t_topic);
				throw Reply(Reply::RPL_TOPIC, r_params);
			}
		}
		// 매개변수가 둘 이상일경우 해당 채널의 토픽을 설정한다.
		else
		{
			// +t 플래그가 활성화인경우 클라이언트가 해당 채널의 관리자인지 검사한다.
			if (channel->checkModeSet('t') == true && channel->isOperator(client) == false)
			{
				r_params.push_back(p_data.channel);
				throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
			}
			// 해당 채널의 토픽을 설정한다.
			channel->setTopic(p_data.topic);

			// 해당 채널의 멤버들에게 토픽이 변경되었다는 메세지를 브로드캐스팅한다.
			{
				std::set< Client* > target_list = channel->getMemberSet();

				r_params.push_back(data.prefix);
				r_params.push_back(data.command);
				r_params.insert(r_params.end(), data.parameters.begin(), data.parameters.end());
				r_params.push_back("\r\n");

				server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
			}
		}
	}
	catch (Reply& reply)
	{
		server.deliverMsg(&client, reply.getReplyMessage(client));
	}

}
