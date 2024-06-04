#include <sys/stat.h>
#include "ClientWriteEventHandler.hpp"
#include "IRC.hpp"
#include "Assert.hpp"
#include "IRC.hpp"

ClientWriteEventHandler::~ClientWriteEventHandler()
{
}

void	ClientWriteEventHandler::interpretEvent(IRC& server, const struct kevent& event)
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
			server.delClient(*client);
			return ;
		}

		client->_writable = false;
		struct kevent	t_event;
		EV_SET(&t_event, client->getSocketFd(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
		server.pushEvent(t_event);
	}
}
