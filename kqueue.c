#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "Assert.h"

#define MAX_EVENTS 1024
#define MAX_CLIENTS 5
#define MAX_MSG_LEN 512

volatile sig_atomic_t g_signo = 0;

void	signal_handler(int signo)
{
	g_signo = signo;
}

struct Client
{
	int	socket;
	struct sockaddr_in	sockaddr;
	char	*buf;
	// need string vector for buf list
};

struct Server
{
	int	socket;
	struct sockaddr_in	sockaddr;

	int	kq;
	int	events_number;
	int	clients_number;
	struct kevent	event_to_monitor;
	struct kevent	events_occurred[MAX_EVENTS];

	struct Client*	clients[MAX_CLIENTS];
};

void	exit_with_sigint(void)
{
	printf("\e[2DServer has shut down\n");
	exit(1);
}

static inline void	stop_sigint_or_fatal(int returned_value, const char* name_of_the_call)
{
	if (g_signo == SIGINT)
		exit_with_sigint();

	if (returned_value == -1)
	{
		perror(name_of_the_call);
		exit(1);
	}
}

static inline void add_client(struct Client* client, struct Server* server)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (server->clients[i] == NULL)
		{
			server->clients[i] = client;
			break;
		}
	}
}

static inline void	add_event_to_kqueue(struct Server* server, uintptr_t ident, int16_t filter, uint16_t flags, struct Client* client)
{
	server->events_number += 1;
	EV_SET(&(server->event_to_monitor), ident, filter, flags, 0, 0, client);

	stop_sigint_or_fatal(kevent(server->kq, &(server->event_to_monitor), 1, NULL, 0, NULL), "kevent");
}

struct sockaddr_in	set_sockaddr_in(int domain, uint16_t port, const char* ip_addr)
{
	struct sockaddr_in	sockaddr;

	sockaddr.sin_family = domain;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = inet_addr(ip_addr);
	if (sockaddr.sin_addr.s_addr == -1)
	{
		fprintf(stderr, "inet_addr: Invalid IP address\n");;
		exit(1);
	}

	return sockaddr;
}

int	setup_socket()
{
	int	_socket;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	stop_sigint_or_fatal(_socket, "socket");

	int	opt = 1;
	stop_sigint_or_fatal(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");

	return _socket;
}

// int	accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
struct Client*	accept_client(int server_socket)
{
	struct Client*	client = malloc(sizeof(struct Client));
	socklen_t	client_socklen = sizeof(struct sockaddr_in);
	client->socket = accept(server_socket, (struct sockaddr*)&(client->sockaddr), &client_socklen);
	fcntl(client->socket, F_SETFL, O_NONBLOCK);
	stop_sigint_or_fatal(client->socket, "accept");

	return client;
}

void	work_server_event(struct Server* server)
{
	struct Client*	client;
	client = accept_client(server->socket);
	add_client(client, server);

	add_event_to_kqueue(server, client->socket, EVFILT_READ, EV_ADD, client);
	add_event_to_kqueue(server, client->socket, EVFILT_WRITE, EV_ADD, client);
	server->clients_number += 1;
}

void	send_msg_to_all_clients(struct Server* server, struct Client* client_with_msg)
{
	for (int i = 0; i < server->clients_number; i++)
	{
		// need string vector for buf list;
		if (server->clients[i] != NULL && server->clients[i] != client_with_msg)
		{
			if ((server->clients[i])->buf)
				free((server->clients[i])->buf);
			(server->clients[i])->buf = strdup(client_with_msg->buf);
			send((server->clients[i])->socket, (server->clients[i])->buf, strlen((server->clients[i])->buf), 0);
		}
	}
}

void	work_client_event(struct kevent* event_occurred, struct Server* server)
{
	struct Client*	client_with_event = (struct Client*)event_occurred->udata;

	if (event_occurred->filter == EVFILT_READ)
	{
		if (client_with_event->buf)
			free(client_with_event->buf);
		client_with_event->buf = malloc(event_occurred->data);
		ssize_t	recv_len = recv(client_with_event->socket, client_with_event->buf, event_occurred->data, 0);
		stop_sigint_or_fatal(recv_len, "recv");

		if (recv_len == 0)
		{
			printf("client with socket %d is disconnected\n", client_with_event->socket);
			close(client_with_event->socket);
			return ;
		}

		client_with_event->buf[recv_len] = '\0';
		send_msg_to_all_clients(server, client_with_event);
	}
	else if (event_occurred->filter == EVFILT_WRITE)
	{
		if (client_with_event == NULL)
		{
			// need string vector for buf list;
			ssize_t	send_len = send(client_with_event->socket, client_with_event->buf, strlen(client_with_event->buf), 0);
			stop_sigint_or_fatal(send_len, "send");
		}
	}
}

// socket create
	// prepare addr
// bind socket to addr
// listen to connections
// set event to the server_socket
// receive events
	// event in server_socket => accept client
		// set event to the newly accepted client_socket
	// event in client_socket => read or write
		// EVFILT_READ -> recv. read the msg and save to the server;
		// EVFILT_WRITE -> send. send msg in the buffer to the clients ready to recv.

int	main()
{
	struct Server	server;
	memset(server.clients, 0, sizeof(server.clients));

	signal(SIGINT, SIG_IGN);
/*
boot the server!
{ */
	server.socket = setup_socket();
	server.sockaddr = set_sockaddr_in(AF_INET, 4242, "127.0.0.1");

	stop_sigint_or_fatal(bind(server.socket, (struct sockaddr*)&(server.sockaddr), sizeof(struct sockaddr_in)), "bind");
	stop_sigint_or_fatal(listen(server.socket, 5), "listen");

// }

	printf("Server has successfully started\n");
	signal(SIGINT, signal_handler);

	server.kq = kqueue();
	stop_sigint_or_fatal(server.kq, "kqueue");

	// listen to connections
	add_event_to_kqueue(&server, server.socket, EVFILT_READ, EV_ADD, NULL);

	// handle signal while calling kevent
	add_event_to_kqueue(&server, SIGINT, EVFILT_SIGNAL, EV_ADD, NULL);

	struct timespec	wait_time;
	wait_time.tv_sec = 3;
	wait_time.tv_nsec = 0;

	int	real_events_number;
	while (true)
	{
		real_events_number = kevent(server.kq, NULL, 0, server.events_occurred, server.events_number, &wait_time);
		stop_sigint_or_fatal(real_events_number, "kevent");

		// handle events
		for (int i = 0; i < real_events_number; i++)
		{
			if (server.events_occurred[i].ident == SIGINT)
				exit_with_sigint();
			else if (server.events_occurred[i].ident == server.socket)
				work_server_event(&server);
			else
				work_client_event(&(server.events_occurred[i]), &server);
		}
	}
}
