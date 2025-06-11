#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define LISTEN_PORT    5400
#define BUFFER_SIZE    100
#define TOTAL_FRAMES   30

typedef enum { PKT_DATA, PKT_ACK } PACKETTYPE;

struct timeval recvTimeout;

typedef struct {
    PACKETTYPE type;
    unsigned int payload_size;
    int frame_no;
    char *payload;
} Packet;

Packet* initPackets()
{
    Packet *pkt = malloc(TOTAL_FRAMES * sizeof(Packet));
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        pkt[i].type         = PKT_DATA;
        pkt[i].frame_no     = i;
        pkt[i].payload      = "$Mbdv,gl;hpoihewadbjlszncx.mvc,bg;/nh''wa?>SF<xdcfvgl;kjhgfdsasedrfg$";
        pkt[i].payload_size = strlen(pkt[i].payload);
    }
    return pkt;
}

int main()
{
    int listenSock, connSock, toSend, received;
    struct sockaddr_in servAddr, cliAddr;
    socklen_t cliLen;
    char sendBuf[BUFFER_SIZE], ackBuf[BUFFER_SIZE];
    Packet *packetArray;
    int expectedAck, sentCount = 0;

    recvTimeout.tv_sec  = 1;
    recvTimeout.tv_usec = 0;

    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port        = htons(LISTEN_PORT);

    bind(listenSock, (struct sockaddr*)&servAddr, sizeof(servAddr));
    listen(listenSock, 5);

    printf("STOP-AND-WAIT SERVER listening on port %d\n\n", LISTEN_PORT);

    packetArray = initPackets();

    printf("How many frames would you like to send? ");
    scanf("%d", &toSend);

    while (sentCount < toSend) {
        printf("\nWaiting for client connection…\n");
        cliLen  = sizeof(cliAddr);
        connSock = accept(listenSock, (struct sockaddr*)&cliAddr, &cliLen);
        printf("Client connected from %s:%d\n",
               inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        while (sentCount < toSend) {
            // set receive timeout for ACK
            if (setsockopt(connSock, SOL_SOCKET, SO_RCVTIMEO,
                           &recvTimeout, sizeof(recvTimeout)) < 0) {
                perror("setsockopt failed");
            }

            // build and send the data packet
            Packet p = packetArray[sentCount];
            snprintf(sendBuf, BUFFER_SIZE, "%d%u%s",
                     p.frame_no, p.payload_size, p.payload);

            if (p.payload_size > 0) {
                send(connSock, sendBuf, p.payload_size, 0);
                printf("Sent Frame %d\n", p.frame_no);

                // wait for ACK
                received = recv(connSock, ackBuf, BUFFER_SIZE, 0);
                if (received <= 0) {
                    printf("No ACK received, retransmitting frame %d\n", p.frame_no);
                } else {
                    int ackNo = ackBuf[0] - '0';
                    printf("Received ACK %d\n", ackNo);
                    if (ackNo == p.frame_no) {
                        sentCount++;
                    }
                }
            }
        }

        close(connSock);
        printf("\nAll %d frames transferred successfully!\n", toSend);
    }

    close(listenSock);
    free(packetArray);
    return 0;
}
