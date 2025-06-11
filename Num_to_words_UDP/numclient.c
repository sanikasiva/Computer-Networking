/*
Write a program to implement a UDP client server system in which the client program sends a number between 1 and 10 and the server program returns the corresponding number in words in an array.
*/


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>
#define PORT 4800
#define MAXLINE 1024

int main()
{
int sockfd;
char buffer[MAXLINE];
char sent[2];
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
printf("Enter a number between 1 and 10 : ");
scanf("%d",&n);
char sn=(char)('0'+n);
sent[0]=sn;
sent[1]='\0';

sendto(sockfd, (char *)sent, strlen(sent),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr));
printf("The number sent to the server is %d\n",n);
n=recvfrom(sockfd,(char *) buffer,MAXLINE,MSG_WAITALL,(struct sockaddr *) &servaddr,&len);
buffer[n]='\0';
printf("The number in words is %s\n",buffer);
close(sockfd);
return 0;
}


/*
CLIENT OUTPUT

cs-db-23@CS-DB-23:~/Sanika/Practice$ gcc numclient.c -o numclient
cs-db-23@CS-DB-23:~/Sanika/Practice$ ./numclient
Enter a number between 1 and 10 : 6
The number sent to the server is 6
The number in words is six
*/
