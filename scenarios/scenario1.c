#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

/**
 * This scenario will test the basic functionality like ADD, UPD and BUY. The server will print the hash_table 
 * after every set of actions. 
 */

int main(int argc, char const *argv[]) {

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8000);

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

    // ADD
    sprintf(msg, "ADD shoes 99");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    sprintf(msg, "ADD pants 100");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    sprintf(msg, "ADD tshirts 101");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    // PRINT
    sprintf(msg, "SER PRINT_HT");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));
    sleep(10);

    // UPD
    sprintf(msg, "UPD shoes 10");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    sprintf(msg, "UPD pants -10");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    // this will fail
    sprintf(msg, "UPD tshirts -102");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    // PRINT
    sprintf(msg, "SER PRINT_HT");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));
    sleep(10);

    // BUY
    sprintf(msg, "BUY shoes 10");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    sprintf(msg, "BUY pants 10");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    // this will fail
    sprintf(msg, "BUY tshirts 102");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    sleep(1);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));

    // PRINT
    sprintf(msg, "SER PRINT_HT");
    send(sock, msg, strlen(msg)+1, 0);
    printf("Client: %s\n", msg);
    memset(buffer, 0, sizeof(buffer));memset(msg, 0, sizeof(msg));
    sleep(5);

    sprintf(msg, "SER KILL");
    send(sock, msg, strlen(msg)+1, 0);

    close(sock);

    return 0;
}