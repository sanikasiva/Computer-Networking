#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int clientSocket;
    struct sockaddr_in serverInfo;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverInfo, 0, sizeof(serverInfo));

    serverInfo.sin_family      = AF_INET;
    serverInfo.sin_port        = htons(3551);
    serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) == -1) {
        perror("Connection failed");
        return EXIT_FAILURE;
    }

    printf("\nCLIENT STARTED  (Individual ACK scheme)\n\n");

    char ackPrefix[50]  = "Acknowledgement of ";
    char ackMessage[50];
    char recvBuff[100];

    for (int expectedSeq = 0; expectedSeq <= 9; expectedSeq++) {
        bzero(recvBuff,  sizeof(recvBuff));
        bzero(ackMessage, sizeof(ackMessage));

        read(clientSocket, recvBuff, sizeof(recvBuff));
        // check sequence number
        int receivedSeq = recvBuff[strlen(recvBuff) - 1] - '0';
        if (receivedSeq != expectedSeq) {
            printf("Discarded frame %d (expected %d).\n\n", receivedSeq, expectedSeq);
            expectedSeq--;  // retry for the same expectedSeq
        }
        else {
            printf("Received from server: %s\n", recvBuff);
            printf("Sending ACK for frame %d.\n\n", receivedSeq);

            strcpy(ackMessage, ackPrefix);
            ackMessage[strlen(ackMessage)]     = (char)(expectedSeq + '0');
            ackMessage[strlen(ackMessage) + 1] = '\0';
            write(clientSocket, ackMessage, sizeof(ackMessage));
        }
    }

    close(clientSocket);
    return 0;
}
