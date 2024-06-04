#include "ClientReadEventHandler.hpp"
#include "IRC.hpp"
#include "Parser.hpp"
#include "Assert.hpp"

ClientReadEventHandler::~ClientReadEventHandler()
{
}

void	ClientReadEventHandler::interpretEvent(IRC& server, const struct kevent& event)
{
	Client*	client = server.searchClient((int)event.ident);
	Assert(client != NULL);

	try
	{
		if ((event.flags & EV_EOF) != 0)
		{
			handleEOF(server, *client);
		}

		char	buf[BUFSIZE];

		int	bytes_received = wrapSyscall(recv(event.ident, buf, BUFSIZE - 1, 0), "recv");
		buf[bytes_received] = '\0';

		IRC::get_next_line(*client, buf);

		IRC::iterate_rdbuf(server, *client);
	}
	catch(enum Client::REGISTER_LEVEL& left)
	{
		client->setRegisterLevel(left);
	}
}

void	ClientReadEventHandler::handleEOF(IRC& server, Client& client) throw(enum Client::REGISTER_LEVEL)
{
	struct kevent	t_event;
	EV_SET(&t_event, client.getSocketFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	server.pushEvent(t_event);

	struct Parser::Data	data = Parser::parseClientMessage("QUIT :Client is disconnected\r\n");
	Command::execute(server, client, data);
}
