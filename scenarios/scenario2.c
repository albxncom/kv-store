#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("ERROR: Run this program like this: ./scenario2 PORT:int\n");
        exit(1);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    char msg[256] = "";
    char buffer[256] = "";


    // BUY item that does not exist
    sprintf(msg, "BUY aslhfskjdfhksd 99");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));


    // If two instances of this program are run at the same time or at least with an delay smaller than 5 seconds,
    // and the SCENARIO_MODE in config.h is set to 1, then the worker will get sleep for 5 seconds
    // everytime it works on something. Nothing really fancy here actually.
    sprintf(msg, "BUY shoes 80");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));


    sprintf(msg, "BUY tshirts 100");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    sprintf(msg, "BUY pants 80");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));
 
    sleep(10);

    close(sock);

    return 0;
}