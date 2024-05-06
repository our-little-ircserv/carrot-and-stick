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
#include <map>

#define PORT 4242
#define MAX_EVENTS 5
#define MAX_CLIENT 100
#define MAX_READ 1000
#define MAX_MESSAGE 512

#define RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[1;33m"
#define EOC "\033[0;0m"

int server_fd, kq;
std::map<int, struct ClientInfo> client_map;
std::vector<struct kevent> change_list;
std::vector<struct kevent> event_list(MAX_EVENTS);

struct ClientInfo
{
	int                         sd;
    bool                        is_writable;
	std::string                 read_buf;
	std::vector<std::string>    write_buf;
};

/*

    sockInfo
    {
        int                 fd
        struct sockaddr_in  addr;
        socklen_t           addrlen;
    }
    // 이 정보는 Server, Client 구분없이 필요로 한다.

*/

// MessegeInfo
// from
// to
// msg

// MessegeQueue
// list of MessegeInfo

/*

    출력 접두사

    [LOG] - 디버깅시 필요한 정보 표시
    [BUG] - 예외상황에 대한 정보 표시
    [INFO] - 사용자에게 필요한 정보 표시

*/

void clientWriteEvent(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo& info = client_map[cur_event.ident];

    sd = info.sd;

    if (info.is_writable == false)
        return ;
    
    const char* msg;
    msg = info.write_buf[0].c_str();

    // 버퍼의 가장 맨 앞 메세지를 하나씩 보낸다, 최적화가 이유
    // 보낸 메세지는 버퍼에서 제거한다
    send(sd, msg, strlen(msg), 0);
    info.write_buf.erase(info.write_buf.begin());
}

void clientWriteCheck(struct ClientInfo& info)
{
    int sd;
    struct kevent event;

    sd = info.sd;

    if (info.is_writable == true && info.write_buf.size() == 0)
    {
        EV_SET(&event, sd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
        info.is_writable = false;

        std::cout << "\n[LOG] ";
        std::cout << "Client " << YELLOW << "( sd: " << sd << " )" << EOC << " writable toggled\n";
        std::cout << "From " << YELLOW << !info.is_writable << EOC << " TO " << YELLOW << info.is_writable << "\n\n" << EOC;

        change_list.push_back(event);
    }
    else if (info.is_writable == false && info.write_buf.size() > 0)
    {
        EV_SET(&event, sd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
        info.is_writable = true;

        std::cout << "\n[LOG] ";
        std::cout << "Client " << YELLOW << "( sd: " << sd << " )" << EOC << " writable toggled\n";
        std::cout << "From " << YELLOW << !info.is_writable << EOC << " TO " << YELLOW << info.is_writable << "\n\n" << EOC;

        change_list.push_back(event);
    }
}

void sendReadToWrite(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo& info = client_map[cur_event.ident];
    std::string msg_str;

    sd = info.sd;
    // 나중에 발신자 정보도 추가한다
    msg_str = info.read_buf;
    info.read_buf = "";

    std::map<int, ClientInfo>::iterator it = client_map.begin();
    std::map<int, ClientInfo>::iterator ite = client_map.end();
    for (; it != ite; it++)
    {
        if (sd == it->second.sd)
            continue;

        it->second.write_buf.push_back(msg_str);
        std::cout << "\n[LOG] ";
        std::cout << "client " << YELLOW << "( sd: " << it->second.sd << " )" << EOC << " receive msg from client " << YELLOW << "( sd: " << sd << " )\n\n" << EOC;
    }
}

void clientReadEvent(struct kevent& cur_event)
{
    int sd;
    struct ClientInfo& info = client_map[cur_event.ident];

    sd = info.sd;

    // 연결 종료
    if (cur_event.data <= 0)
    {
        std::cout << GREEN << "\n[INFO] ";
        std::cout << EOC << "Client " << YELLOW << "( sd: " << sd << " )" << RED << " disconnected" << EOC << "...\n\n";

        std::map<int, struct ClientInfo>::iterator it = client_map.find(sd);
        client_map.erase(it);
        close(sd);

        return ;
    }
	// 데이터 읽기
    else
    {
        std::cout << EOC << "\n[LOG] ";
        std::cout << "ReadEvent from sd: " << sd << "\n\n";

        int buf_size;
        buf_size = cur_event.data;
        if (cur_event.data > MAX_READ)
            buf_size = MAX_READ;
        char buffer[buf_size + 1];
        int bytes_read = read(sd, buffer, buf_size);
        buffer[bytes_read] = '\0';

        info.read_buf += std::string(buffer);
        return ;
    }
}

void serverReadEvent()
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_addrlen;
	struct kevent event;
	struct ClientInfo info;

    std::stringstream ss;

	// 새로운 연결 수락
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
	if (client_fd < 0) {
		exit(EXIT_FAILURE);
	}

    info.sd = client_fd;
    info.is_writable = false;
    info.read_buf = "";

    client_map[client_fd] = info;

	// 새 클라이언트에 대한 이벤트 추가
	EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	change_list.push_back(event);

	EV_SET(&event, client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	change_list.push_back(event);

    // 유저가 연결되었다는 log를 기록
    std::cout << GREEN << "\n[INFO] ";
    std::cout << EOC << "New Client " << YELLOW << "( sd: " << client_fd << " )" << EOC << " has connected Server!\n\n";

    // 해당 client에게 환영 메세지를 보냄
    ss << "[Server] Welcome New Client! Now Your sd: " << YELLOW << client_fd << "\n" << EOC;
    client_map[client_fd].write_buf.push_back(ss.str());
}

void init(struct sockaddr_in& address, socklen_t& addrlen, int& kq)
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
    addrlen = sizeof(address);

    // 소켓 바인드
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
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
	change_list.push_back(event);
}

int main()
{
	struct sockaddr_in server_addr;
	socklen_t server_addrlen;

	init(server_addr, server_addrlen, kq);

    std::cout << GREEN << "[LOG] ";
	std::cout << GREEN << "Server is listening on port " << YELLOW << PORT << GREEN << "...\n\n" << EOC;

    while (true)
	{
        timespec ts;
        ts.tv_sec = 3;
        ts.tv_nsec = 0;
        int n = kevent(kq, &(*change_list.begin()), change_list.size(), &(*event_list.begin()), MAX_EVENTS, &ts);
        std::cout << "[LOG] ";
        std::cout << "number of detected events: " << YELLOW << n << "\n" << EOC;

        if (n < 0)
        {
            perror(strerror(errno));
            exit(EXIT_FAILURE);
        }
		
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

                    // disconnect시에 별도의 추가 진행없이 바로 다음 루프로 넘어간다
                    if (event_list[i].data == 0)
                        continue ;

                    // 해당 클라이언트의 read buf를 파싱한다
                    // command를 수행한다

                    // 다른 클라이언트의 write buf를 채운다
                    sendReadToWrite(event_list[i]);
                }
            }
        }

        // 현재 write_buf에 따라 write event를 toggle 할 client가 있는지 확인한다
        // event_list에서 찾으려고 했던 실수 -> 이벤트 발생전까지는 토글을 할 수 없음 ㅋㅋ;
        {
            std::map<int, ClientInfo>::iterator it = client_map.begin();
            std::map<int, ClientInfo>::iterator ite = client_map.end();
            for (; it != ite; it++)
            {
                clientWriteCheck(it->second);
            }
        }

        // 이벤트 감지 여부와 상관없이 send 가능한 메세지들은 send를 수행한다.
        // 이 부분은 추후 MessegeQueue 에서 일괄적으로 담당한다
        for (int i = 0; i < n; i++)
        {
            int sd = event_list[i].ident;

            if (sd == server_fd || event_list[i].filter != EVFILT_WRITE)
                continue;

            clientWriteEvent(event_list[i]);
        }
    }

    close(server_fd);
    return 0;
}
