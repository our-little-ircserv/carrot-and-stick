#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "IRC.hpp"
#include "Error.hpp"
#include "Signal.hpp"

extern volatile sig_atomic_t g_signo;
extern int	kq;

IRC::IRC() : eventlist(1)
{
}

IRC::~IRC()
{
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		delete it->second;
	}

	clients.clear();
}

void	IRC::handleEvents(Network& network_handler)
{
	struct kevent	event;
	struct timespec	time;
	time.tv_sec = 3;
	time.tv_nsec = 0;

	kq = wrapSyscall(kqueue(), "kqueue");
	EV_SET(&event, network_handler.getServerSocketFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	changelist.push_back(event);

	while (true)
	{
		int	real_events = wrapSyscall(kevent(kq, &(changelist.front()), changelist.size(), &(*eventlist.begin()), MAX_EVENTS, &time), "kevent");

		changelist.clear();

		if (real_events == 0)
		{
			std::cout << "No events ready" << std::endl;
			continue ;
		}

		for (int i = 0; i < real_events; i++)
		{
			if ((int)eventlist[i].ident == network_handler.getServerSocketFd())
			{
				acceptClient(network_handler);
			}
			else
			{
				recvSendMesssages(&eventlist[i]);
			}
		}
	}
}

void	IRC::acceptClient(Network& network_handler)
{
	struct kevent	event;
	struct sockaddr_in	client_addr;
	socklen_t	socklen = sizeof(struct sockaddr_in);

	Client*	accepted_client = new Client(wrapSyscall(accept(network_handler.getServerSocketFd(), (struct sockaddr*)&client_addr, &socklen), "accept"));
	clients[accepted_client->getSocketFd()] = accepted_client;

	EV_SET(&event, accepted_client->getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	changelist.push_back(event);
	EV_SET(&event, accepted_client->getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	changelist.push_back(event);
}

void	IRC::recvSendMesssages(struct kevent* event_occurred)
{
	if ((int)event_occurred->filter == EVFILT_READ)
	{
		std::string received_msg = receiveMessages(event_occurred);
		// parse received_msg

		if (clients[event_occurred->ident])
		{
//			std::cout << clients[event_occurred->ident]->getSocketFd() << " - " << received_msg << std::endl;
		}

		// fill write buf to other clients
		// other clients: EVFILT_WRITE, EV_ADD | EV_ENABLE
	}
	else if ((int)event_occurred->filter == EVFILT_WRITE)
	{
		// send write buf
		// send clients: EVFILT_WRITE, EV_ADD | EV_DISABLE
	}
}

std::string	IRC::receiveMessages(struct kevent* event_occurred)
{
	std::map<int, Client*>::iterator	it = clients.find((int)event_occurred->ident);
	if (it == clients.end())
	{
		return "";
	}

	Client*	client = it->second;
	char	buf[event_occurred->data];

	ssize_t recv_len = recv(client->getSocketFd(), buf, event_occurred->data, 0);
	wrapSyscall(recv_len, "recv");
	if (recv_len == 0)
	{
		close(client->getSocketFd());
		clients.erase(client->getSocketFd());
		delete client;
	}

	buf[recv_len] = '\0';
	return buf;
}
