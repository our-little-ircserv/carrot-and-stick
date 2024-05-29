#include <sys/stat.h>
#include "ClientEventHandler.hpp"
#include "IRC.hpp"
#include "Parser.hpp"
#include "Assert.hpp"

ClientEventHandler::~ClientEventHandler()
{
}

void	ClientEventHandler::read(IRC& server, const struct kevent& event) throw(Signal, FatalError, enum Client::REGISTER_LEVEL)
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

void	ClientEventHandler::write(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	struct stat	statbuf;
	if (fstat(event.ident, &statbuf) == -1)
	{
		return;
	}

	Client*	client = server.searchClient((int)event.ident);
	Assert(client != NULL);

	size_t	sent_size;
	while (client->_write_buf.empty() == false)
	{
		size_t	data_size = client->_write_buf[0].size();
		sent_size = wrapSyscall(send(event.ident, client->_write_buf[0].c_str(), data_size, 0), "send");
		if (sent_size < data_size)
		{
			client->_write_buf[0] = client->_write_buf[0].substr(sent_size);
			break;
		}

		client->_write_buf.erase(client->_write_buf.begin());
	}

	if (client->_write_buf.empty() == true && client->_writable == true)
	{
		if (client->getRegisterLevel() == Client::LEFT)
		{
			server.disconnectClient(*client);
		}
	}
}

void	ClientEventHandler::handleEOF(IRC& server, Client& client) throw(enum Client::REGISTER_LEVEL)
{
	struct kevent	t_event;
	EV_SET(&t_event, client.getSocketFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	server._changelist.push_back(t_event);

	struct Parser::Data	data = Parser::parseClientMessage("QUIT :Client is disconnected\r\n");
	Command::execute(server, client, data);
}
