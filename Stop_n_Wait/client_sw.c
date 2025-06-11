#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define SERVER_PORT    5400
#define SERVER_ADDR    "127.0.0.1"
#define BUFFER_SIZE    100
#define TOTAL_FRAMES   30

typedef enum { PKT_DATA, PKT_ACK } PACKETTYPE;

typedef struct {
    PACKETTYPE type;
    unsigned int payload_size;
    int frame_no;
    char *payload;
} Packet;

int main()
{
    int sockfd, bytesRecv;
    struct sockaddr_in servAddr;
    char recvBuf[BUFFER_SIZE], ackMsg[BUFFER_SIZE];
    int expectedFrame = 0, deliveredCount = 0;

    // connect to server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    servAddr.sin_port        = htons(SERVER_PORT);

    if (connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Connect failed");
        return EXIT_FAILURE;
    }

    printf("STOP-AND-WAIT CLIENT connected to %s:%d\n\n",
           SERVER_ADDR, SERVER_PORT);

    while (deliveredCount < TOTAL_FRAMES) {
        bytesRecv = recv(sockfd, recvBuf, BUFFER_SIZE, 0);
        if (bytesRecv <= 0) {
            printf("Server disconnected or error\n");
            break;
        }

        int recvFrameNo = recvBuf[0] - '0';
        if (recvFrameNo == expectedFrame) {
            // deliver to application
            printf("Received Frame %d\n", recvFrameNo);

            // send ACK
            snprintf(ackMsg, BUFFER_SIZE, "%d", recvFrameNo);
            // simulate processing delay
            srand(time(NULL));
            sleep((rand() % 4) + 1);

            send(sockfd, ackMsg, strlen(ackMsg) + 1, 0);
            printf("Sent ACK %d\n\n", recvFrameNo);

            expectedFrame++;
            deliveredCount++;
        }
        else {
            printf("Discarded duplicate Frame %d (expected %d)\n\n",
                   recvFrameNo, expectedFrame);
        }
    }

    close(sockfd);
    return 0;
}
