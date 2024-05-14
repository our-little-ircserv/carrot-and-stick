#include "Parser.hpp"
#include "Command.hpp"

struct Command::Join	Parser::join(const std::vector< std::string >& params) throw(Error)
{
	struct Command::Join	data;

	if (params.size() < 2)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENEPARM, concat_params.c_str());
	}

	size_t				i = 0;
	size_t				offset;
	const std::string&	t_channels = params[0];
	std::string			channel_name;
	while (i < t_channels.size())
	{
		offset = t_channels.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_channels.size();
		}
		offset -= i;

		if (offset > 50)
		{
			offset = 50;
		}

		channel_name = t_channels.substr(i, offset);
		if (Parser::isValidChannelName(channel_name) == false)
		{
			throw Error(Error::EWRPARM, channel_name.c_str());
		}

		data.channels.push_back(channel_name);
		i += offset + 1;
	}

	const std::string&	t_keys = params[1];
	std::string			key_value;
	i = 0;
	while (t_keys[i] != '\0')
	{
		offset = t_keys.find_first_of(',', i);
		if (offset == std::string::npos)
		{
			offset = t_keys.size();
		}
		offset -= i;

		key_value = t_keys.substr(i, offset);
		data.keys.push_back(key_value);
		i += offset + 1;
	}

	return data;
}

void	Command::join(IRC& server, Client& client, const std::vector< std::string >& params) throw (Error)
{
	struct Command::Join data;

	data = ::Parser::join(params);
	/*

	채널목록을 순회하며
	1. 채널이 존재하는지 확인한다, 없다면 새로 생성한다. (이름이 여럿일때 유효하지 않은 이름이 섞여있는 경우 동작 확인할것)
	2. 채널의 모드를 확인한다.(모드간 우선순위 확인할것)
	3. 모드에 따른 확인 절차를 거치고 멤버를 추가한다.

	isExistChannel
	true -> getChannel -> reference? 초기화가 아니면 값 대입이 불가능 -> pointer*

	*/
	size_t chan_len;

	chan_len = data.channels.size();

	for (size_t i = 0; i < chan_len; i++)
	{
		if (Parser::isValidChannelName(data.channels[i]) == false)
			continue ;

		
		Channel* channel = server.searchChannel(data.channels[i]);

		if (channel == NULL)
		{
			channel = server.createChannel(client, static_cast<Channel::Prefix>(data.channels[i][0]), data.channels[i]);
			continue ;
		}

		// 채널에 들어가기 위한 유효성 검사

		// 1. 초대여부
		if (channel->checkModeSet('i') == true)
		{
			//
		}

		// 2. 비밀번호
		if (channel->checkModeSet('k') == true)
		{
			if ((channel->getKey() == data.keys[i]) == false)
			{
				// 에러처리
				continue ;
			}
		}

		// 3. 인원제한
		if (channel->checkModeSet('l') == true)
		{
			if (channel->getLimit() == channel->getMemberCnt())
			{
				// 에러처리
				continue ;
			}
		}

		channel->addMember(client);
	}	
}
