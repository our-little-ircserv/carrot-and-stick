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
#include <vector>
#include <deque>
#include <algorithm>

#define PORT 4242
#define MAX_EVENTS 5
#define MAX_CLIENT 100
#define MAX_READ 1000
#define MAX_MESSAGE 512

int server_fd, kq;
std::deque<struct ClientInfo*> client_deq;
std::vector<struct kevent> change_list;
std::vector<struct kevent> event_list(MAX_EVENTS);

struct ClientInfo
{
	int sd;
	std::string read_buf;
    bool is_writable = false;
	std::deque<std::string> write_buf;
};

// MessegeInfo
// from
// to
// msg

// MessegeQueue
// list of MessegeInfo

void clientWriteEvent(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo* info;

    info = (struct ClientInfo*)cur_event.udata;
    sd = info->sd;

    if (info->is_writable == false)
        return ;

    // 버퍼의 가장 맨 앞 메세지를 하나씩 보낸다, 최적화가 이유
    // 보낸 메세지는 버퍼에서 제거한다
    send(sd, (info->write_buf[0]).c_str(), info->write_buf[0].size(), 0);
    info->write_buf.erase(info->write_buf.begin());
}

void clientWriteCheck(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo* info;
    struct kevent event;

    info = (struct ClientInfo*)cur_event.udata;
    sd = info->sd;

    if (info->is_writable == true && info->write_buf.size() == 0)
        EV_SET(&event, sd, EVFILT_WRITE, EV_DISABLE, 0, 0, info);
    else if (info->is_writable == false && info->write_buf.size() > 0)
        EV_SET(&event, sd, EVFILT_WRITE, EV_ENABLE, 0, 0, info);

    change_list.emplace_back(event);
}

void clientReadEvent(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo* info;

    info = (struct ClientInfo*)cur_event.udata;
    sd = info->sd;

    // 연결 종료
    if (cur_event.data <= 0)
    {
        std::deque<struct ClientInfo*>::iterator it = std::find(client_deq.begin(), client_deq.end(), info);
        client_deq.erase(it);
        delete info;
        close(sd);

        return ;
    }
	// 데이터 읽기
    else
    {
        int buf_size;
        if (cur_event.data > MAX_READ)
            buf_size = MAX_READ;
        char buffer[buf_size + 1];
        int bytes_read = read(sd, buffer, buf_size);
        buffer[bytes_read] = '\0';

        info->read_buf += std::string(buffer);

        return ;
    }
}

void serverReadEvent()
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_addrlen;
	struct kevent event;
	struct ClientInfo* info;

	// 새로운 연결 수락
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
	if (client_fd < 0) {
		exit(EXIT_FAILURE);
	}

	info = new struct ClientInfo;
	client_deq.push_back(info);

	// 새 클라이언트에 대한 이벤트 추가
	EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, info);
	change_list.emplace_back(event);

	EV_SET(&event, client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, info);
	change_list.emplace_back(event);
}

void init(struct sockaddr_in& address, socklen_t& addrlen, int kq)
{
	struct kevent event;

	// 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 주소 초기화
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");;
    address.sin_port = htons(PORT);

    // 소켓 바인드
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 리스닝 시작
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

	// kqueue 생성
    if ((kq = kqueue()) == -1) {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    // 소켓을 감시할 이벤트 추가
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	change_list.emplace_back(event);
}

int main()
{
	struct sockaddr_in server_addr;
	socklen_t server_addrlen;

	init(server_addr, server_addrlen, kq);

	std::cout << "Server is listening on port " << PORT << "...\n";

    while (true)
	{
        int n = kevent(kq, &(*change_list.begin()), change_list.size(), &(*event_list.begin()), MAX_EVENTS, NULL);

        if (n < 0)
            exit(EXIT_FAILURE);
		
		change_list.clear();

        for (int i = 0; i < n; i++)
        {
            int sd = event_list[i].ident;

            // server의 event
            if (sd == server_fd)
			{
                serverReadEvent();
            }
            // client의 event
            else
            {
                if (event_list[i].filter == EVFILT_READ)
                {
                    clientReadEvent(event_list[i]);

                    // 해당 클라이언트의 read buf를 파싱한다
                    // command를 수행한다
                        // 다른 클라이언트의 write buf를 채운다
                }
                clientWriteCheck(event_list[i]);
            }
        }

        // 이벤트 감지 여부와 상관없이 send 가능한 메세지들은 send를 수행한다.
        // 이 부분은 추후 MessegeQueue 에서 일괄적으로 담당한다
        for (int i = 0; i < n; i++)
        {
            int sd = event_list[i].ident;

            if (sd == server_fd)
                continue;

            clientWriteEvent(event_list[i]);
        }
    }

    close(server_fd);
    return 0;
}
