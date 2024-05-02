#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <sstream>

#define PORT 4242
#define MAX_CLIENT 3

// messege 의 주인 빼고 모든 client 에게 send 를 수행한다
void sendForAllClient(int* clientList, int listSize, int hostSd, const char* messege)
{
	for (int i = 0; i < listSize; i++)
	{
		if (clientList[i] == 0)
			continue ;
		if (clientList[i] != hostSd)
			send(clientList[i], messege, strlen(messege), 0);
	}
}

int main()
{
	int serverFd;
	fd_set fs;
	int clientFd[MAX_CLIENT];
	int clientCnt = 0;
	int maxSd;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char buf[1024] = {0};
	const char* echo = "Echo from server: ";

	for (int i = 0; i < MAX_CLIENT; i++)
		clientFd[i] = 0;

	serverFd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT);
	addrlen = sizeof(addr);
	if (bind(serverFd, (struct sockaddr *)&addr, addrlen) == -1)
	{
		perror(strerror(errno));
		exit(1);
	}

	listen(serverFd, 3);

	std::cout << "Server is ready\n";
	std::cout << "Waiting for client's connection...\n";

	while (true)
	{
		int sd, selRet;

		// fd_set 초기화
		FD_ZERO(&fs);
		// server socket 의 fd 는 무조건적으로 추가해둔다
		FD_SET(serverFd, &fs);
		maxSd = serverFd;

		// 접속해있는 client 들의 socket 을 fd set 에 추가 및 select 에 쓰일 높은 fd 를 찾는다
		for (int i = 0; i < MAX_CLIENT; i++)
		{
			sd = clientFd[i];
			if (sd != 0)
			{
				// 할당된 client socket을 fd set 에 추가한다
				FD_SET(sd, &fs);
				// 가장높은 fd 를 갱신한다
				if (sd > maxSd)
					maxSd = sd;
			}
		}

		// 0 부터 차례대로 확인할 max fd, 읽기용 fd set, 쓰기용 fd set, 예외 fd set, timeout
		// 새 접속, 새 메세지, 기타등등 아무 이벤트나 기다린다
		selRet = select(maxSd + 1, &fs, NULL, NULL, NULL);

		if (selRet < 0)
		{
			perror(strerror(errno));
			exit(1);
		}

		// 새 접속 처리, 서버쪽에서 이벤트가 감지된다
		if (FD_ISSET(serverFd, &fs))
		{
			int newClient;
			const char* notice;
			std::stringstream ss;

			// 새 client 의 fd 를 배열에 추가해준다
			newClient = accept(serverFd, (struct sockaddr *)&addr, &addrlen);
			// 더 이상 client 를 수용 가능한 공간이 없을 경우
			if (clientCnt >= MAX_CLIENT)
			{
				ss << "Connection is Fail !!\nServer is Full\n";
				notice = ss.str().c_str();
				send(newClient, notice, strlen(notice), 0);

				close(newClient);
			}
			// clientFd 배열에 socket discriptor 를 추가한다
			else
			{
				for (int i = 0; i < MAX_CLIENT; i++)
				{
					if (clientFd[i] == 0)
					{
						clientFd[i] = newClient;
						clientCnt++;

						ss << "Connection is Successful !!\nNow You are Client [" << i << "]\n";
						notice = ss.str().c_str();
						ss.str("");
						send(newClient, notice, strlen(notice), 0);

						ss << "Client [" << i << "] has been connected !!\n";
						notice = ss.str().c_str();
						ss.str("");
						sendForAllClient(clientFd, MAX_CLIENT, newClient, notice);

						std::cout << notice;

						break ;
					}
				}
			}
		}

		// 접속한 client 들의 이벤트를 검사
		for (int i = 0; i < MAX_CLIENT; i++)
		{
			std::stringstream ss;
			const char* messege;

			sd = clientFd[i];
			// 새 이벤트가 있다면, 메세지가 들어왔을 경우...
			if (FD_ISSET(sd, &fs))
			{
				int readSize = recv(sd, buf, 1024, 0);
				
				// eof 혹은 다른 이유로 client 의 접속이 끊어진경우
				if (readSize == 0)
				{
					clientFd[i] = 0;
					close(sd);

					ss << "Client [" << i << "] has been left...\n";
					messege = ss.str().c_str();
					sendForAllClient(clientFd, MAX_CLIENT, -1, messege);

					std::cout << messege;

					clientCnt--;
					if (clientCnt == 0)
						break ;
				}
				// 이벤트를 발동시킨 client 를 제외한 모두에게 messege 를 보낸다
				else
				{
					buf[readSize] = '\0';
					ss << "Msg from client[" << i << "]: " << buf;
					messege = ss.str().c_str();
					sendForAllClient(clientFd, MAX_CLIENT, sd, messege);

					std::cout << messege;
				}
			}
		}

		if (clientCnt == 0)
		{
			std::cout << "ALL Clients has been left...\n";
			close(serverFd);
			break ;
		}
	}
}