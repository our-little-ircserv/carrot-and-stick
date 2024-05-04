#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "IRC.hpp"
#include "Error.hpp"
#include "Signal.hpp"

extern volatile sig_atomic_t g_signo;
extern int	kq;

IRC::IRC() : eventlist(1), writable(false)
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

void	IRC::runIRC(Network& network_manager) throw(Signal, Error)
{
	struct kevent	event;

	kq = wrapSyscall(kqueue(), "kqueue");
	EV_SET(&event, network_manager.getServerSocketFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	changelist.push_back(event);
	handleEvents(network_manager);
}

void	IRC::handleEvents(Network& network_manager) throw(Signal, Error)
{
	struct timespec	time;
	time.tv_sec = 3;
	time.tv_nsec = 0;

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
			if ((int)eventlist[i].ident == network_manager.getServerSocketFd())
			{
				acceptClient(network_manager);
			}
			else
			{
				handleMesssages(&eventlist[i]);
			}
		}
	}
}

void	IRC::acceptClient(Network& network_manager) throw(Signal, Error)
{
	struct kevent	event;
	struct sockaddr_in	client_addr;
	socklen_t	socklen = sizeof(struct sockaddr_in);

	Client*	accepted_client = new Client(wrapSyscall(accept(network_manager.getServerSocketFd(), (struct sockaddr*)&client_addr, &socklen), "accept"));
	clients[accepted_client->getSocketFd()] = accepted_client;

	EV_SET(&event, accepted_client->getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	changelist.push_back(event);
	EV_SET(&event, accepted_client->getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	changelist.push_back(event);
}

void	IRC::deleteClient(Client* client) throw(Signal, Error)
{
	wrapSyscall(close(client->getSocketFd()), "close");
	clients.erase(client->getSocketFd());
	delete client;
}

void	IRC::handleMesssages(struct kevent* event_occurred) throw(Signal, Error)
{
	std::map<int, Client*>::iterator	it = clients.find((int)event_occurred->ident);
	if (it == clients.end())
	{
		return;
	}

	Client*	client = it->second;
	if ((int)event_occurred->filter == EVFILT_READ)
	{
		std::string received_msg = receiveMessages(it, event_occurred);
		if (received_msg.empty())
		{
			deleteClient(client);
			return;
		}

		client->read_buf.push_back(received_msg);
		addReceivedMessagesToWriteBuffers(it);
		client->read_buf.clear();
	}
	else if ((int)event_occurred->filter == EVFILT_WRITE)
	{
		sendMessages(client);
		client->write_buf.clear();

		struct kevent	event;
		EV_SET(&event, client->getSocketFd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
		changelist.push_back(event);
		writable = false;
	}
}

std::string	IRC::receiveMessages(std::map<int, Client*>::iterator& it, struct kevent* event_occurred) throw(Signal, Error)
{
	Client*	client = it->second;
	char	buf[event_occurred->data];

	ssize_t recv_len = recv(client->getSocketFd(), buf, event_occurred->data, 0);
	wrapSyscall(recv_len, "recv");

	buf[recv_len] = '\0';
	return buf;
}

void	IRC::sendMessages(Client* client) throw(Signal, Error)
{
	for (std::vector<std::string>::iterator it = client->write_buf.begin(); it != client->write_buf.end(); it++)
	{
		wrapSyscall(send(client->getSocketFd(), it->c_str(), it->length(), 0), "send");
	}
}

void	IRC::addReceivedMessagesToWriteBuffers(std::map<int, Client*>::iterator& msg_owner)
{
	struct kevent	event;
	std::vector<std::string>& owner_rdbuf = msg_owner->second->read_buf;

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it != msg_owner)
		{
			Client*	client = it->second;
			moveReadBufferToWriteBuffer(owner_rdbuf, client->write_buf);
			if (writable == false)
			{
				EV_SET(&event, client->getSocketFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				changelist.push_back(event);
				writable = true;
			}
		}
	}
}

void	IRC::moveReadBufferToWriteBuffer(std::vector<std::string>& rdbuf, std::vector<std::string>& wrbuf)
{
	for (std::vector<std::string>::iterator it = rdbuf.begin(); it != rdbuf.end(); it++)
	{
		wrbuf.push_back(*it);
	}
}
