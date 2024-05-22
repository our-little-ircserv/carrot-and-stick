#include "Parser.hpp"
#include "Command.hpp"

struct	Command::Topic	Parser::topic(const Client& client, const std::vector< std::string >& params) throw(Reply)
{
	struct	Command::Topic		data;
	std::vector< std::string >	r_params;

	if (params.size() < 1)
	{
		r_params.push_back(client.getNickname());
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

	p_data = ::Parser::topic(client, data.parameters);
	/*
		1. 채널 조회
		2. 존재하는 채널의 경우 토픽 존재여부에 따라 토픽 조회
		3. 토픽 설정의 경우 채널 멤버인지 먼저 검사
		4. 채널 멤버인경우 +t 플래그에 따라 토픽 설정 여부 분기
	*/
	channel = server.searchChannel(p_data.channel);

	// 존재하지 않는 채널
	if (channel == NULL)
	{
		r_params.push_back(client.getNickname());
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
	}

	// 단순 토픽 조회
	if (p_data.topic.empty() == true)
	{
		std::string t_topic;

		t_topic = channel->getTopic();

		// 서버에서 다음 토픽들을 메세지로 클라이언트에게 전달
		if (t_topic.size() == 0)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(p_data.channel);
			throw Reply(Reply::RPL_NOTOPIC, r_params);
		}
		else
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(p_data.channel);
			r_params.push_back(channel->getTopic());
			throw Reply(Reply::RPL_TOPIC, r_params);
		}
	}
	// 토픽 지정
	else
	{
		// 해당 채널의 멤버인지 검사
		if (channel->isMember(client) == false)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(p_data.channel);
			throw Reply(Reply::ERR_NOTONCHANNEL, r_params);
		}
		// T플래그가 활성화인경우 관리자인지 검사
		else if (channel->checkModeSet('t') == true && channel->isOperator(client) == false)
		{
			r_params.push_back(client.getNickname());
			r_params.push_back(p_data.channel);
			throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
		}
		channel->setTopic(p_data.topic);

		// 서버에서 해당 채널 구성원들에게 브로드캐스트
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
