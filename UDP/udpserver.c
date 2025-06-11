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
struct sockaddr_in servaddr,cliaddr;
if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
{
perror("Socket Creation Failed");
exit(EXIT_FAILURE);
}
memset(&servarvaddr.sin_port=htons(PORT);
servaddr.sin_addr.s_addr=INADDR_ANY;
if(bind(sockfd,(const struct sockaddr *) &servaddr,sizeof(servaddr))<0)
{
perror("Bind Failed");
exit(EXIT_FAILURE);
}
int n,len;
len=sizeof(cliaddr);
n=recvfrom(sockfd,(char *) buffer,MAXLINE,MSG_WAITALL,(struct sockaddr *) &cliaddr,&len);
buffer[n]='\0';
printf("Client Time: %s\n",buffer);
sendto(sockfd,(const char *)hello,strlen(hello),MSG_CONFIRM,(const struct sockaddr *)&cliaddr,len);
printf("\n");
return 0;
}
