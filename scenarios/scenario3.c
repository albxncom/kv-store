#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(int argc, char const *argv[]) {
    if (argc != 5) {
        printf("ERROR: Run this program like this: ./scenario3 PORT:int MAX_DELAY:int RANGE_ITEM_ID:int HOW_MANY_REQS:int\n");
        exit(1);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};
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

    int rand_item_id;
    int rand_amount;
    int action;
    char msg[256] = "";
    int i=0;

    srand(time(0));

    while(1) {
        if (atoi(argv[2]) != 0) {
            usleep(atoi(argv[2]));
        }

        rand_item_id = rand() % atoi(argv[3]);

        // either buy or get, 50/50% chance
        action = rand() % 2;
        if(action == 0) {
            // BUY
            rand_amount = rand() % 20;
            sprintf(msg, "BUY item_%i %i\n", rand_item_id, rand_amount);
        } else {
            // GET
            sprintf(msg, "GET item_%i\n", rand_item_id);
        }
        //printf("%s", msg);

        send(sock, msg, strlen(msg)+1, 0);

        i++;
        if(i==atoi(argv[4])) break;
    }

    sleep(5);
    close(sock);

    return 0;
}