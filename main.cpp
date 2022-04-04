#include "pch.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include <thread>
#include <mutex>


// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base());
    return s;
}

std::mutex m;
bool is_do = true;
std::string str = "";
bool is_started_server = false;

int Server_Process(int port) {

    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket sock = { AF_INET, SOCK_DGRAM, IPPROTO_UDP };

    std::cout << "Starting echo server on the port " << port << "...\n";

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return EXIT_FAILURE;
    }

    char buffer[1024];

    // socket address used to store client address
    struct sockaddr_in client_address = { 0 };
    int client_address_len = sizeof(sockaddr_in);
    ssize_t recv_len = 0;

    std::cout << "Running echo server...\n" << std::endl;
    is_started_server = true;  // сервер запустился, можно разрешать ввод команды exit
    char client_address_buf[INET_ADDRSTRLEN];

    while (is_do)
    {
        // Read content into buffer from an incoming client.
        recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&client_address, &client_address_len);

        if (recv_len > 0)
        {
            buffer[recv_len] = '\0';
            std::cout
                << "Client with address "
                << inet_ntop(AF_INET, &client_address.sin_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
                << ":" << ntohs(client_address.sin_port)
                << " sent datagram "
                << "[length = "
                << recv_len
                << "]:\n'''\n"
                << buffer
                << "\n'''"
                << std::endl;

            sendto(sock, buffer, recv_len, 0, reinterpret_cast<const sockaddr*>(&client_address),
                client_address_len);

         // ======  блок задания 1: "Дополните реализованный сервер обратным резолвом имени клиента"
            DWORD dwRetval;
            char hostname[NI_MAXHOST];
            char servInfo[NI_MAXSERV];
            dwRetval = getnameinfo((struct sockaddr*)&client_address,
                sizeof(struct sockaddr),
                hostname,
                NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

            if (dwRetval != 0) {
                printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
                return 1;
            }
            else {
                printf("getnameinfo returned hostname = %s\n", hostname);
                return 0;
            }
            // ======  конец блокa задания 1

        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}

bool async_in(int p) {
    while (!is_started_server)    {}  // запускаем ввод строки только когда запустился сервер
 
    std::cout << "Enter command:" << std::endl;
    std::cin >> str;
    if (str == "exit") {
        m.lock();
        is_do = false;
        m.unlock();
        return false;
    }

    return true;
}

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    int port { std::stoi(argv[1]) };

        // вынес сервер в отдельную функцию Server_Process

    // создаём 2 потока, один для сервера, другой - для ввода команды exit (задание 2)

    std::thread th1(Server_Process, port);     // поток для сервера
    std::thread th2(async_in, port);           // поток для команды exit

    th1.join();
    th2.join();

    return 0;
}

