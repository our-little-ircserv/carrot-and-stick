#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <cctype>
# include <string>
# include <vector>
# include "Reply.hpp"
# include "IRC.hpp"

namespace	Parser
{
	struct Data;
};

class	IRC;

namespace Command
{
	enum CmdType
	{
		PASS,
		NICK,
		USER,
		NO_SUCH_COMMAND = -1
	};
	
	enum ModeType
	{
		DEL,
		ADD
	};

	struct	Join
	{
		std::vector< std::string >	channels;
		std::vector< std::string >	keys;
	};

	struct	Privmsg
	{
		std::vector< std::string >	msg_targets;
		std::string					text_to_be_sent;
	};

	struct	User
	{
		std::string	username;
		std::string	real_name;
	};

	struct	Topic
	{
		std::string	channel;
		std::string	topic;
	};

	struct	Invite
	{
		std::string	nickname;
		std::string	channel;
	};

	struct	Kick
	{
		std::vector< std::string >	channels;
		std::vector< std::string >	users_nick;
		std::string					comment;
	};

	struct	ModeWithParams
	{
		enum ModeType	type;
		char			mode;
		std::string		mode_param;
	};

	struct	Mode
	{
		std::string										channel;
		std::vector< struct Command::ModeWithParams >	modes;
		// addMode로 모드 설정할 때
		// modes iterator->mode를 전부 하나의 문자열로 합친 다음
		// +ikl 꼴로 만들어서 addMode 해준다...
	};

	static std::string CmdList[7] = {
		"PASS", "JOIN", "TOPIC", "INVITE", "NICK", "KICK", "MODE"
	};

	static std::vector< void (*)(IRC&, Client&, const struct Parser::Data&) > cmdFunctions;

	// 추후에 함수 배열 추가

	void	init();
	int		getType(std::string& command);
	void	execute(IRC& server, Client& client, struct ::Parser::Data& data);

	// command implementations
	void	pass(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	join(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	mode(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	topic(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	invite(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	nick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	kick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
};

#endif
