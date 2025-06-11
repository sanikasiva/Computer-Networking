#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

int main()
{
    int serverSocket, connectionSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(3551);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        return EXIT_FAILURE;
    }

    printf("\tSERVER\n Go-Back-N (window=3) sending 10 messages\n\n");
    listen(serverSocket, 10);
    addrLen = sizeof(clientAddr);
    connectionSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

    char  baseMessage[50] = "Message ";
    char  recvBuffer[50];
    int   seqNum    = -1;
    fd_set readSet1, readSet2;
    struct timeval timeout1, timeout2;
    int   selRes1, selRes2;

send_batch_start:
    // send first two frames of the window
    seqNum++;
    char sendBuffer[60];
    bzero(sendBuffer, sizeof(sendBuffer));
    strcpy(sendBuffer, baseMessage);
    sendBuffer[strlen(sendBuffer)]     = (char)(seqNum + '0');
    sendBuffer[strlen(sendBuffer) + 1] = '\0';
    printf("Sent to client: %s\n\n", sendBuffer);
    write(connectionSocket, sendBuffer, sizeof(sendBuffer));
    usleep(1000);

    seqNum++;
    bzero(sendBuffer, sizeof(sendBuffer));
    strcpy(sendBuffer, baseMessage);
    sendBuffer[strlen(sendBuffer)]     = (char)(seqNum + '0');
    sendBuffer[strlen(sendBuffer) + 1] = '\0';
    printf("Sent to client: %s\n\n", sendBuffer);
    write(connectionSocket, sendBuffer, sizeof(sendBuffer));
    usleep(1000);

send_third:
    // send third frame of the window
    bzero(sendBuffer, sizeof(sendBuffer));
    strcpy(sendBuffer, baseMessage);
    sendBuffer[strlen(sendBuffer)]     = (char)(seqNum + '0');
    sendBuffer[strlen(sendBuffer) + 1] = '\0';
    printf("Sent to client: %s\n\n", sendBuffer);
    write(connectionSocket, sendBuffer, sizeof(sendBuffer));

    // wait for ACK of this batch
    FD_ZERO(&readSet1);
    FD_SET(connectionSocket, &readSet1);
    timeout1.tv_sec  = 2;
    timeout1.tv_usec = 0;

    selRes1 = select(connectionSocket + 1, &readSet1, NULL, NULL, &timeout1);
    if (selRes1 < 0) {
        perror("select error");
    }
    else if (selRes1 == 0) {
        // timeout: slide back window
        printf("Timeout! Resending from frame %d\n\n", seqNum - 2);
        seqNum -= 3;
        goto send_batch_start;
    }
    else {
        // got ACK
        bzero(recvBuffer, sizeof(recvBuffer));
        read(connectionSocket, recvBuffer, sizeof(recvBuffer));
        printf("Received from client: %s\n\n", recvBuffer);
        seqNum++;
        if (seqNum <= 9) {
            goto send_third;
        }
    }

ack_remaining:
    // after all batches, finalize remaining ACKs
    FD_ZERO(&readSet2);
    FD_SET(connectionSocket, &readSet2);
    timeout2.tv_sec  = 3;
    timeout2.tv_usec = 0;

    selRes2 = select(connectionSocket + 1, &readSet2, NULL, NULL, &timeout2);
    if (selRes2 < 0) {
        perror("select error");
    }
    else if (selRes2 == 0) {
        // if any final ACKs missing, retransmit last two frames
        printf("Final ACK timeout! Resending last two frames starting at %d\n\n", seqNum - 2);
        seqNum -= 2;

        bzero(sendBuffer, sizeof(sendBuffer));
        strcpy(sendBuffer, baseMessage);
        sendBuffer[strlen(sendBuffer)]     = (char)(seqNum + '0');
        sendBuffer[strlen(sendBuffer) + 1] = '\0';
        write(connectionSocket, sendBuffer, sizeof(sendBuffer));
        usleep(1000);

        seqNum++;
        bzero(sendBuffer, sizeof(sendBuffer));
        strcpy(sendBuffer, baseMessage);
        sendBuffer[strlen(sendBuffer)]     = (char)(seqNum + '0');
        sendBuffer[strlen(sendBuffer) + 1] = '\0';
        write(connectionSocket, sendBuffer, sizeof(sendBuffer));
        goto ack_remaining;
    }
    else {
        // receive final two ACKs
        bzero(recvBuffer, sizeof(recvBuffer));
        read(connectionSocket, recvBuffer, sizeof(recvBuffer));
        printf("Received from client: %s\n\n", recvBuffer);

        bzero(recvBuffer, sizeof(recvBuffer));
        read(connectionSocket, recvBuffer, sizeof(recvBuffer));
        printf("Received from client: %s\n\n", recvBuffer);
    }

    close(connectionSocket);
    close(serverSocket);
    return 0;
}
