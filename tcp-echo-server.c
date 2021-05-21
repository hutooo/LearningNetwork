#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 100
// const int BUFFER_SIZE = 100;
#define error_quit(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int main(int argc, char *argv[]) {
    if (argc < 2) {error_quit("Usage: %s [port]\n", argv[0]);}

    int port = atoi(argv[1]);

    int server_fd, client_fd, err;
    struct sockaddr_in srv_addr, cli_addr;
    char buf[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error_quit("socket create failed");
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    if (bind(server_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
        error_quit("could not bind socket");
    }

    if (listen(server_fd, 128) < 0) {
        error_quit("Could not listen on socket\n");
    }

    printf("server start, listening on %d\n", port);

    while (1) {
        socklen_t client_len = sizeof(cli_addr);

        if ((client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &client_len)) < 0) {
            error_quit("could not establish new connection\n");
        }

        while (true) {
            int read = recv(client_fd, buf, BUFFER_SIZE, 0);
            if (!read) {break;}
            if (read < 0) {error_quit("read failed\n");}
            if (send(client_fd, buf, read, 0) < 0) {error_quit("write failed\n");}
        }
    }
}