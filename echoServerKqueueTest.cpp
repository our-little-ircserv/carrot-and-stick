// 이 코드는 GPT 4.0 의 도움을 받아 작성되었습니다.

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <sstream>

#define PORT 4242
#define MAX_EVENTS 5

int main() {
    int serverFd, client_fd, kq;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    struct kevent event_set[MAX_EVENTS];
    struct kevent event;
	const char* echo = "Echo from server: ";

    // 소켓 생성
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 주소 초기화
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");;
    address.sin_port = htons(PORT);

    // 소켓 바인드
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 리스닝 시작
    if (listen(serverFd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // kqueue 생성
    if ((kq = kqueue()) == -1) {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    // 소켓을 감시할 이벤트 추가
    EV_SET(&event, serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        exit(EXIT_FAILURE);
    }

	std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        int n = kevent(kq, NULL, 0, event_set, MAX_EVENTS, NULL);
        if (n < 0) {
            perror("kevent");
            continue;
        }

        for (int i = 0; i < n; i++) {
            int sd = event_set[i].ident;

            if (sd == serverFd) {
                // 새로운 연결 수락
                client_fd = accept(serverFd, (struct sockaddr *)&address, &addrlen);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                // 새 클라이언트에 대한 이벤트 추가
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    perror("kevent");
                    close(client_fd);
                }
            } else if (event_set[i].filter == EVFILT_READ) {
                // 데이터 읽기
                char buffer[1024];
                int bytes_read = read(sd, buffer, sizeof(buffer));
                if (bytes_read <= 0) {
                    close(sd);
                    continue;
                }
                buffer[bytes_read] = '\0';

				std::cout << "Msg from client[" << sd << "]: " << buffer;

				write(sd, echo, strlen(echo));
                write(sd, buffer, bytes_read);  // 에코
            }
        }
    }

    close(serverFd);
    return 0;
}