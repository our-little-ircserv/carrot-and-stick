#ifndef REPLY_HPP
# define REPLY_HPP

# include <vector>
# include <string>

class	Reply
{
	public:
		enum	ReplyType
		{
			RPL_WELCOME = 001,
			RPL_YOURHOST = 002,
			RPL_CREATED = 003,
			RPL_MYINFO = 004,
			RPL_CHANNELMODEIS = 324,
			RPL_NOTOPIC = 331,
			RPL_TOPIC = 332,
			RPL_INVITING = 341,

			ERR_NOSUCHNICK = 401,
			ERR_NOSUCHCHANNEL = 403,
			ERR_TOOMANYTARGETS = 407,
			ERR_NORECIPIENT = 411,
			ERR_NOTEXTTOSEND = 412,
			ERR_NONICKNAMEGIVEN = 431,
			ERR_ERRONEUSNICKNAME = 432,
			ERR_NICKNAMEINUSE = 433,
			ERR_USERNOTINCHANNEL = 441,
			ERR_NOTONCHANNEL = 442,
			ERR_USERONCHANNEL = 443,
			ERR_NEEDMOREPARAMS = 461,
			ERR_ALREADYREGISTRED = 462,
			ERR_KEYSET = 467,
			ERR_CHANNELISFULL = 471,
			ERR_UNKNOWNMODE = 472,
			ERR_INVITEONLYCHAN = 473,
			ERR_BADCHANNELKEY = 475,
			ERR_NOCHANMODES = 477,
			ERR_CHANOPRIVSNEEDED = 482
		};

		Reply(enum Reply::ReplyType t_number, const std::vector< std::string >& t_parameters);
		std::string	getReplyMessage();
	
	private:
		enum ReplyType				_number;
		std::vector< std::string >	_parameters;
};

#endif
