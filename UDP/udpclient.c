#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>
#define PORT 8081
#define MAXLINE 1024

int main()
{
int sockfd;
char buffer[MAXLINE];
time_t t;
time(&t);
char *hello=ctime(&t);
ctime(&t);
struct sockaddr_in servaddr;
if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
{
perror("Socket Creation Failed");
exit(EXIT_FAILURE);
}
memset(&servaddr,0,sizeof(servaddr));
servaddr.sin_family=AF_INET;
servaddr.sin_port=htons(PORT);
servaddr.sin_addr.s_addr=INADDR_ANY;
int n,len;
sendto(sockfd, (const char *)hello, strlen(hello),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr));
printf("\n");
n=recvfrom(sockfd,(char *) buffer,MAXLINE,MSG_WAITALL,(struct sockaddr *) &servaddr,&len);
buffer[n]='\0';
printf("Server Time: %s\n",buffer);
close(sockfd);
return 0;
}
