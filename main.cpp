
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

#define PORT 9090
#define MAX_WAITING_CONNECTIONS 10
typedef struct sockaddr_in socket_adress_t;
enum LOG_LEVEL
{
    ERROR,
    WANING,
    SUCESS,
    INFO
};
inline void log(std::string message, LOG_LEVEL level);
int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == -1)
    {
        log("socket not initiated", LOG_LEVEL::ERROR);
        return -1;
    }
    log("socket intiniated sucessfully", LOG_LEVEL::SUCESS);
    socket_adress_t socket_adress;
    socket_adress.sin_port = htons(PORT);
    socket_adress.sin_family = AF_INET;
    socket_adress.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int bind_state = bind(socket_fd, (struct sockaddr *)&socket_adress, sizeof(socket_adress));
    if (bind_state < 0)
    {
        log("binde faild", LOG_LEVEL::ERROR);
        close(socket_fd);
        return -1;
    }
    log("port binded successfully", LOG_LEVEL::SUCESS);
    // 3. Listen for connections (backlog queue size = 10)
    if (listen(socket_fd, MAX_WAITING_CONNECTIONS) < 0)
    {
        log("Listen failed on binded port faild", LOG_LEVEL::ERROR);
        return -1;
    }
    int current_client_fd;
    // accepting clients
    while (true)
    {
        current_client_fd = accept(socket_fd, nullptr, nullptr);

        if (current_client_fd == -1)
        {
            log("cannot accept client", LOG_LEVEL::ERROR);
            return -1;
        }
        // accepting tcp connection fromthe currently connected client
        char buffer[2048];
        ssize_t current_readed_bytes_length;
        do
        {
            current_readed_bytes_length = recv(current_client_fd, buffer, sizeof(buffer), 0);
            if (current_readed_bytes_length == -1)
            {
                log("cannot read data from request", LOG_LEVEL::ERROR);
                close(current_client_fd);

                return -1;
            }
            if (!current_readed_bytes_length)
            {
                log("client exited", LOG_LEVEL::INFO);
                close(current_client_fd);
                break;
            }
            std::cout << "current_readed_bytes_length = " << current_readed_bytes_length << std::endl;
            buffer[current_readed_bytes_length] = '\0';
            log("recived tcp buffer data is :" + (std::string)buffer, LOG_LEVEL::INFO);

        } while (true);
    }
    return 1;
}

inline void log(std::string message, LOG_LEVEL level)
{
    std::cout << "[";
    switch (level)
    {
    case 0:
        std::cout << "ERROR";
        break;
    case 1:
        std::cout << "WARNING";
        break;
    case 2:
        std::cout << "SUCCESS";
        break;
    case 3:
        std::cout << "INFO";
        break;
    }
    std::cout << "]: " << message << std::endl;
}