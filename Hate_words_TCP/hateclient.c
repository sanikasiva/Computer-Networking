/*
Implement a TCP client server program for performing message verification operations. 
Client sends a file containing a text to the server, display the result returned by server. Server checks if the text sent by the client contains hate words. If yes, return status is rejected, else return status is approved. Server should maintain the list of hate words.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6505
#define MAX_BUFFER_SIZE 1024

void send_file_to_server(const char *file_name) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send the file name to the server
    send(client_socket, file_name, strlen(file_name), 0);

    // Receive the response from the server
    int len = recv(client_socket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';  // Null-terminate the received string
        printf("Server response: %s\n", buffer);
    } else {
        printf("Error receiving response from server.\n");
    }

    close(client_socket);
}

int main() {
    char file_name[MAX_BUFFER_SIZE];

    // Get the file name from the user
    printf("Enter the file name to send to the server: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = 0;  // Remove trailing newline character

    // Send the file name to the server
    send_file_to_server(file_name);

    return 0;
}


/*
CLIENT OUTPUT

cs-db-23@CS-DB-23:~/Sanika/Practice$ ./hateclient
Enter the file name to send to the server: hatefile.txt
Server response: Rejected
*/
