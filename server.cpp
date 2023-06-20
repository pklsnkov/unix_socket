#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

const int BUFSIZE = 64;
const int MAXFILENAME = 32;

int main() {
    int socket_dp, socket_connect, filename_len, file_dp, buflen;
    const char* str_path = "./servsoc";
    sockaddr_un serv_addr, user_addr;
    char buf[BUFSIZE], filename[MAXFILENAME];

    socklen_t struct_len = sizeof(user_addr);

    memset(&serv_addr, 0, sizeof(serv_addr));
    strcpy(serv_addr.sun_path, str_path);
    serv_addr.sun_family = AF_UNIX;

    if ((socket_dp = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка серверного сокета");
        exit(1);
    }

    unlink(str_path);

    if (bind(socket_dp, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("Присвоить имя сокету не удалось");
        exit(1);
    }

    if (listen(socket_dp, 3) < 0) {
        perror("Ошибка очереди запросов");
        exit(1);
    }

    while (true) {
        if ((socket_connect = accept(socket_dp, reinterpret_cast<sockaddr*>(&user_addr), &struct_len)) < 0) {
            perror("Ошибка запроса");
            exit(1);
        }

        if ((filename_len = read(socket_connect, filename, MAXFILENAME)) < 0) {
            perror("Ошибка имени");
            exit(1);
        }

        filename[filename_len] = '\0';

        if ((file_dp = open(filename, O_RDONLY)) < 0) {
            perror("Ошибка открытия файла");
            exit(1);
        }

        memset(buf, 0, BUFSIZE);

        while ((buflen = read(file_dp, buf, BUFSIZE)) != 0) {
            write(socket_connect, buf, buflen);
            memset(buf, 0, BUFSIZE);
        }

        close(file_dp);
        close(socket_connect);
    }

    return 0;
}