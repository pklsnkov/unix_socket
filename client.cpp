#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

const int BUFSIZE = 64;
const int MAXFILENAME = 32;

int main(int argc, char *argv[]) {
    int socket_dp, buflen;
    const char *str_path = ".servsoc";
    struct sockaddr_un serv_addr;
    char buf[BUFSIZE], filename[MAXFILENAME];

    std::strcpy(serv_addr.sun_path, str_path);
    serv_addr.sun_family = AF_LOCAL;

    if ((socket_dp = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания локального сокета");
        exit(1);
    }

    if (connect(socket_dp, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("Ошибка при соединении с сокетом сервера");
        exit(1);
    }

    std::strcpy(filename, argv[1]);

    if (write(socket_dp, filename, std::strlen(argv[1])) < 0) {
        perror("Ошибка при передачи имени серверу");
        exit(1);
    }

    std::memset(buf, 0, BUFSIZE);

    while ((buflen = read(socket_dp, buf, BUFSIZE)) != 0) {
        write(1, buf, buflen);
        std::memset(buf, 0, BUFSIZE);
    }
    
    close(socket_dp);

    return 0;
}
