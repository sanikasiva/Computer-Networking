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
char reqnum[MAXLINE];
struct sockaddr_in servaddr,cliaddr;
if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
{
perror("Socket Creation Failed");
exit(EXIT_FAILURE);
}
memset(&servaddr,0,sizeof(servaddr));
memset(&cliaddr,0,sizeof(cliaddr));
servaddr.sin_family=AF_INET;
servaddr.sin_port=htons(PORT);
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


char h = buffer[0];
int t=(int)(h-'0');
printf("Number sent by client: %d\n",t);
switch(t)
{
  case 1 : strcpy(reqnum,"one");
                        break;
  case 2 : strcpy(reqnum,"two");
                        break;
  case 3 : strcpy(reqnum,"three");
                        break;                        
  case 4 : strcpy(reqnum,"four");
                        break;
  case 5 : strcpy(reqnum,"five");
                        break;
  case 6 : strcpy(reqnum,"six");
                          break;                        
  case 7 : strcpy(reqnum,"seven");
                        break;
  case 8 : strcpy(reqnum,"eight");
                        break;
  case 9 : strcpy(reqnum,"nine");
                        break;
  case 10 : strcpy(reqnum,"ten");
                        break;
  default: printf("Enter a number from 1 to 10: ");
}

sendto(sockfd,(char *)reqnum,strlen(reqnum),MSG_CONFIRM,(const struct sockaddr *)&cliaddr,len);
printf("The number in words is: %s\n",reqnum);
return 0;
}

/*
SERVER OUTPUT

cs-db-23@CS-DB-23:~/Sanika/Practice$ gcc numserver.c -o numserver
cs-db-23@CS-DB-23:~/Sanika/Practice$ ./numserver
Number sent by client: 6
The number in words is: six
*/


