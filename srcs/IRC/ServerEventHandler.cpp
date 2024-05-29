#include "ServerEventHandler.hpp"

ServerEventHandler::~ServerEventHandler()
{
}

void	ServerEventHandler::read(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
	struct kevent		t_event;
	struct sockaddr_in	client_addr;
	socklen_t			socklen = sizeof(struct sockaddr_in);

	int client_fd = wrapSyscall(accept(server.getServerSocketFd(), (struct sockaddr*)&client_addr, &socklen), "accept");
	Client	client(client_fd, client_addr);

	server._clients[client.getSocketFd()] = client;

	EV_SET(&t_event, client.getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, (void*)(&(server._client_event_handler)));
	server._changelist.push_back(t_event);

	EV_SET(&t_event, client.getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void*)(&(server._client_event_handler)));
	server._changelist.push_back(event);
}

void	ServerEventHandler::write(IRC& server, const struct kevent& event) throw(Signal, FatalError)
{
}
