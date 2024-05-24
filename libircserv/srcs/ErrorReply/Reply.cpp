#include <sstream>
#include <iomanip>
#include "Reply.hpp"
#include "IRC.hpp"
#include "Parser.hpp"

Reply::Reply(enum Reply::ReplyType t_number, const std::vector< std::string >& t_parameters) : _number(t_number), _parameters(t_parameters)
{
}

// :hostname rpl_no parameters :message crlf
std::string	Reply::getReplyMessage()
{
	std::stringstream	ss;

	ss << std::setw(3) << std::setfill('0') << _number;
	std::string	message = ":" + IRC::hostname + " " + ss.str() + " " + _parameters[0] + " ";
	_parameters.erase(_parameters.begin());

	switch (_number)
	{
		case RPL_WELCOME:
			// hostname 1 nickname ...
			message += ":Welcome to the Internet Relay Network <nick>!<user>@<host>";
			break;
		case RPL_YOURHOST:
			// hostname 2 nickname ...
			message += ":Your host is carrot-and-stick.irc.kr, running version 0.0.1";
			break;
		case RPL_CREATED:
			// hostname 3 nickname ...
			message += ":This server was created <date>";
			break;
		case RPL_MYINFO:
			// hostname 4 nickname hostname hostversion avail_usermodes avail_channelmodes
			message += "carrot-and-stick.irc.kr 0.0.1 0 +oitkl";
			break;
		case RPL_CHANNELMODEIS:
			// hostname 324 nickname #channel +it
			message += Parser::concat_string_vector(_parameters);
			break;
		case RPL_NOTOPIC:
			// hostname 331 nickname #channel :No topic is set
			message += _parameters[0] + " :No topic is set";
			break;
		case RPL_TOPIC:
			// hostname 332 nickname #channel :topic
			message += _parameters[0] + " :" + _parameters[1];
			break;
		case RPL_INVITING:
			// hostname 341 nickname #channel user_to_invite
			message += Parser::concat_string_vector(_parameters);
			break;
		case ERR_NOSUCHNICK:
			// hostname 401 nickname nick/channel :No such nick/channel
			message += _parameters[0] + " :No such nick/channel";
			break;
		case ERR_NOSUCHCHANNEL:
			// hostname 401 nickname channel :No such channel
			message += _parameters[0] + " :No such channel";
			break;
		case ERR_TOOMANYTARGETS:
			// hostname 407 nickname ... :Too many recipients
			message += Parser::concat_string_vector(_parameters, ',') + " :Too many recipients";
			break;
		case ERR_NORECIPIENT:
			// hostname 411 nickname :No recipient given (command)
			message += ":No recipient given (" + _parameters[0] + ")";
			break;
		case ERR_NOTEXTTOSEND:
			// hostname 412 nickname :No text to send
			message += ":No text to send";
			break;
		case ERR_UNKNOWNCOMMAND:
			// hostname 411 nickname <command> :Unknown command
			message += _parameters[0] + " :Unknown command";
			break;
		case ERR_NONICKNAMEGIVEN:
			// hostname 431 nickname/* :No nickname given
			message += ":No nickname given";
			break;
		case ERR_ERRONEUSNICKNAME:
			// hostname 432 nickname/* invalid_nickname :Erroneous nickname
			message += _parameters[0] + " :Erroneous nickname";
			break;
		case ERR_NICKNAMEINUSE:
			// hostname 433 nickname/* invalid_nickname :Nickname is already in use
			message += _parameters[0] + " :Nickname is already in use";
			break;
		case ERR_USERNOTINCHANNEL:
			// hostname 441 nickname user #channel :They aren't on that channel
			message += Parser::concat_string_vector(_parameters) + " :They aren't on that channel";
			break;
		case ERR_NOTONCHANNEL:
			// hostname 442 nickname #channel :You're not on that channel
			message += _parameters[0] + " :You're not on that channel";
			break;
		case ERR_USERONCHANNEL:
			// hostname 443 nickname user #channel :is already on channel
			message += Parser::concat_string_vector(_parameters) + " :is already on channel";
			break;
		case ERR_NOTREGISTERED:
			// hostname 411 nickname :You have not registered
			message += " :You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			// hostname 461 nickname command :Not enough parameters
			message += _parameters[0] + " :Not enough parameters";
			break;
		case ERR_ALREADYREGISTRED:
			// hostname 462 nickname :Unauthorized command (already registered)
			message += ":Unauthorized command (already registered)";
			break;
		case ERR_KEYSET:
			// hostname 467 nickname #channel :Channel key already set
			message += _parameters[0] + " :Channel key already set";
			break;
		case ERR_CHANNELISFULL:
			// hostname 471 nickname #channel :Cannot join channel (+l)
			message += _parameters[0] + " :Cannot join channel (+l)";
			break;
		case ERR_UNKNOWNMODE:
			// hostname 472 nickname c :is unknown mode char to me for #channel
			message += _parameters[0] + " :is unknown mode char to me for " + _parameters[1];
			break;
		case ERR_INVITEONLYCHAN:
			// hostname 473 nickname #channel :Cannot join channel (+i)
			message += _parameters[0] + " :Cannot join channel (+i)";
			break;
		case ERR_BADCHANNELKEY:
			// hostname 475 nickname #channel :Cannot join channel (+k)
			message += _parameters[0] + " :Cannot join channel (+k)";
			break;
		case ERR_NOCHANMODES:
			// hostname 477 nickname #channel :Channel doesn't support modes
			message += _parameters[0] + " :Channel doesn't support modes";
			break;
		case ERR_CHANOPRIVSNEEDED:
			// hostname 482 nickname #channel :You're not channel operator
			message += _parameters[0] + " :You're not channel operator";
			break;
	}

	message += "\r\n";

	return message;
}
