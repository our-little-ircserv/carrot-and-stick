#include <arpa/inet.h>
#include "ServerReadEventHandler.hpp"
#include "IRC.hpp"

ServerReadEventHandler::~ServerReadEventHandler()
{
}

void	ServerReadEventHandler::interpretEvent(IRC& server, const struct kevent& event)
{
	(void)event;
	struct sockaddr_in	client_addr;
	socklen_t			socklen = sizeof(struct sockaddr_in);

	int client_fd = wrapSyscall(accept(event.ident, reinterpret_cast< struct sockaddr* >(&client_addr), &socklen), "accept");

	Client&	client = server.createClient(client_fd, client_addr);

	struct kevent		t_event;
	EV_SET(&t_event, client.getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast< void* >(&(server.getClientReadEventHandler())));
	server.pushEvent(t_event);

	EV_SET(&t_event, client.getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	server.pushEvent(t_event);
}
