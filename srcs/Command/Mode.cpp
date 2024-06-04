#ifdef DEBUG
# include <iostream>
#endif
#include "Parser.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Assert.hpp"

struct Command::Mode	Parser::mode(const std::vector< std::string >& params) throw(Reply)
{
	struct Command::Mode		data;
	std::vector< std::string >	r_params;
	size_t	params_size = params.size();

	if (params_size < 1)
	{
		r_params.push_back("MODE");
		throw Reply(Reply::ERR_NEEDMOREPARAMS, r_params);
	}

	data.channel = params[0];

	size_t	i = 1;
	while (i < params_size)
	{
		struct Command::ModeWithParams	t_mode_with_params;

		size_t new_modes = 0;
		if (Parser::isMode(params[i][0]) == true)
		{
			new_modes = Parser::insertModes(data, params[i], params[i][0]);
			++i;
		}

		if (i < params_size && Parser::isMode(params[i][0]) == false)
		{
			i = Parser::insertModeParameters(data, params, new_modes, i);
		}
	}

	return data;
}

// oitkl
void	Command::mode(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply)
{
	struct Command::Mode		p_data = Parser::mode(data.parameters);
	std::vector< std::string >	r_params;

	Channel*	channel = server.searchChannel(p_data.channel);
	// 1. 채널 존재 여부
	if (channel == NULL)
	{
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_NOSUCHCHANNEL, r_params);
	}
	// 2. 모드 조회 (외부인도 조회 가능)
	if (p_data.modes.empty() == true)
	{
		r_params.push_back(p_data.channel);
		r_params.push_back(channel->getCurrentMode());
		r_params.push_back(channel->getCurrentModeParam(channel->isMember(client)));
		throw Reply(Reply::RPL_CHANNELMODEIS, r_params);
	}
	// 3. 모드 변경 (채널 오퍼레이터 권한 확인)
	if (channel->isMember(client) == false || channel->isOperator(client) == false)
	{
		r_params.push_back(p_data.channel);
		throw Reply(Reply::ERR_CHANOPRIVSNEEDED, r_params);
	}


	// 채널 오퍼레이터만 모드를 변경할 수 있다.

	std::string	t_modes_set_add;
	std::string	t_modes_set_del;
	std::vector< std::string >	t_params_set_add;
	std::vector< std::string >	t_params_set_del;

	std::vector< struct Command::ModeWithParams >::iterator it = p_data.modes.begin();
	std::vector< struct Command::ModeWithParams >::iterator ite = p_data.modes.end();
	while (it != ite)
	{
#ifdef DEBUG
		std::cout << it->mode << ": " << it->mode_param << std::endl;
#endif
		// 채널에 모드 설정을 요청한다.
		try
		{
			channel->setMode(*it);
			if (it->type == Command::ADD)
			{
				t_modes_set_add += it->mode;
				if (channel->checkModeSet(it->mode) == true \
						&& it->mode != 'i' && it->mode != 't' && (it->mode != 'k' || channel->isMember(client) == true))
				{
					t_params_set_add.push_back(it->mode_param);
				}
			}
			else
			{
				t_modes_set_del += it->mode;
				if (it->mode == 'o')
				{
					t_params_set_del.push_back(it->mode_param);
				}
			}
		}
		// 채널에서 모드 설정을 거부할 경우 에러 메시지를 전송한다.
		catch (const Reply& e)
		{
			server.deliverMsg(&client, e.getReplyMessage(client));
		}

		++it;
	}

	if (t_modes_set_add.empty() == false)
	{
		t_modes_set_add.insert(0, "+");
	}

	if (t_modes_set_del.empty() == false)
	{
		t_modes_set_del.insert(0, "-");
	}

	// 성공한 설정들만 모아서 브로드캐스트한다.
	if (t_modes_set_add.empty() == false || t_modes_set_del.empty() == false)
	{
		r_params.push_back(data.prefix);
		r_params.push_back(data.command);
		Assert(data.parameters.empty() == false);
		r_params.push_back(data.parameters.front());

		r_params.push_back(t_modes_set_add);
		r_params.push_back(Parser::concat_string_vector(t_params_set_add));
		r_params.push_back(t_modes_set_del);
		r_params.push_back(Parser::concat_string_vector(t_params_set_del));

		r_params.push_back("\r\n");

		std::set< Client* > target_list = channel->getMemberSet();
		server.deliverMsg(target_list, Parser::concat_string_vector(r_params));
	}
}
