#include "Parser.hpp"
#include "Command.hpp"

struct Command::Invite	Parser::invite(const std::vector< std::string >& params) throw(Error)
{
	struct Command::Invite	data;

	if (params.size() < 2)
	{
		std::string	concat_params = Parser::concat_string_vector(params);
		throw Error(Error::ENEPARM, concat_params.c_str());
	}

	data.nickname = params[0];
	data.channel = params[1];

	return data;
}

void	Command::invite(IRC& server, Client& client, const std::vector< std::string >& params) throw (Error)
{
	/*
		초대하는 자가 해당 채널에 접속중인지 확인 - 해당 채널이 실존하는지는 검사하지 않음
		초대받는 자가 실존하는 유저인지 검사
		초대받는 자의 client객체를 구해야하니 getClient 메서드 사용 - nickname으로 검색
		초대받는 자가 해당 채널에 이미 존재하는지 확인
		invite_list에서 중복확인후 추가 -> 추후 join시에 invite_list에서 제거하는것 검토
		채널 매개변수가 여러개여도 첫번째것만 적용
	*/
}
