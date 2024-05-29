#include <sys/socket.h>
#include <arpa/inet.h>
#include "ServerEventHandler.hpp"
#include "IRC.hpp"

ServerEventHandler::~ServerEventHandler()
{
}

void	ServerEventHandler::read(IRC& server, const struct kevent& event) throw(Signal, FatalError, enum Client::REGISTER_LEVEL)
{
	(void)event;
	struct sockaddr_in	client_addr;
	socklen_t			socklen = sizeof(struct sockaddr_in);
	Client*				t_client;

	int client_fd = wrapSyscall(accept(server.getServerSocketFd(), reinterpret_cast< struct sockaddr* >(&client_addr), &socklen), "accept");

	t_client = server.createClient(client_fd, client_addr);

	struct kevent		t_event;
	EV_SET(&t_event, t_client->getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast< void* >(&(server.getClientEventHandler())));
	server.pushEvent(t_event);

	EV_SET(&t_event, t_client->getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, reinterpret_cast< void* >(&(server.getClientEventHandler())));
	server.pushEvent(t_event);
}

void	ServerEventHandler::write(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	(void)server;
	(void)event;
}
