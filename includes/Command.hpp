#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <vector>
# include <string>
# include "Reply.hpp"
# include "IRC.hpp"

namespace	Parser
{
	struct Data;
}

class	Channel;

namespace Command
{
	enum CmdType
	{
		QUIT,
		PASS,
		NICK,
		USER,
		JOIN,
		TOPIC,
		INVITE,
		KICK,
		MODE,
		PRIVMSG,
		PART,
		UNKNOWNCOMMAND = -1
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
	};

	struct	Part
	{
		std::vector< std::string >	channels;
		std::string					comment;
	};

	static std::string CmdList[12] = {
		"QUIT", "PASS", "NICK", "USER", "JOIN", "TOPIC", "INVITE", "KICK", "MODE", "PRIVMSG", "PART", "NAMES"
	};

	static std::vector< void (*)(IRC&, Client&, const struct Parser::Data&) > cmdFunctions;

	// 추후에 함수 배열 추가

	void	init();
	int		getType(std::string& command);
	void	execute(IRC& server, Client& client, struct ::Parser::Data& data);

	// command implementations
	void	pass(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	join(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	user(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	mode(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	topic(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	invite(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	nick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	kick(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	mode(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	privmsg(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	part(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	quit(IRC& server, Client& client, const struct Parser::Data& data) throw(enum Client::REGISTER_LEVEL);
	void	names(IRC& server, Client& client, const struct Parser::Data& data) throw(Reply);
	void	handleClientDepartment(IRC& server, Channel* channel, Client& client);
}


#endif
