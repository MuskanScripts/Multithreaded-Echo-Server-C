// Simple Multithreaded TCP Echo Server in C
// Compile: gcc -pthread server.c -o server
// Run: ./server <port_number>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void* client_handler(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Null terminate string
        printf("Received: %s", buffer);

        // Echo back to client
        write(client_sock, buffer, bytes_read);
    }

    printf("Client disconnected.\n");
    close(client_sock);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_sock, *client_sock_ptr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s...\n", argv[1]);

    while (1) {
        int *client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("malloc");
            continue;
        }

        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (*client_sock < 0) {
            perror("accept");
            free(client_sock);
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, client_sock) != 0) {
            perror("pthread_create");
            free(client_sock);
            continue;
        }
        pthread_detach(tid);  // Resources freed on thread exit
    }

    close(server_sock);
    return 0;
}
