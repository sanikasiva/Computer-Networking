#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 4474
#define MAX_BUFFER_SIZE 1024
char str2[MAX_BUFFER_SIZE];
void reverse(char * str)
{
    
    int j=0;
	for(int i=strlen(str)-1;i>=0;i--)
	{
		str2[j]=str[i];
		j++;
	}
	str2[j]='\0';
	printf("Reversed string : %s\n",str2);
	
}	

// Function to check if a string is a palindrome
int is_palindrome(const char* str) {
    int left = 0, right = strlen(str) - 1;

    while (left < right) {
        // Ignore non-alphanumeric characters and convert to lowercase
        if (!isalnum(str[left])) {
            left++;
        } else if (!isalnum(str[right])) {
            right--;
        } else if (tolower(str[left]) != tolower(str[right])) {
            return 0; // Not a palindrome
        } else {
            left++;
            right--;
        }
    }

    return 1; // Is a palindrome
}

// Function to trim leading and trailing spaces or newlines
void trim(char* str) {
    int start = 0, end = strlen(str) - 1;

    // Trim leading spaces/newlines
    while (str[start] == ' ' || str[start] == '\n' || str[start] == '\r') {
        start++;
    }

    // Trim trailing spaces/newlines
    while (end >= start && (str[end] == ' ' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }

    // Shift the string to the front
    for (int i = 0; i <= end - start; i++) {
        str[i] = str[start + i];
    }

    str[end - start + 1] = '\0'; // Null terminate the string
}

int main() {
    int server_socket, client_socket, read_size;
    struct sockaddr_in server, client;
    char buffer[MAX_BUFFER_SIZE];
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    listen(server_socket, 3);

    printf("Server listening on port %d...\n", PORT);

    // Accept and handle incoming connections
    while (1) {
        // Accept connection
        int client_address_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr*)&client, (socklen_t*)&client_address_size);
        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connected to client: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Receive file from client
        FILE* file = fopen("output.txt", "w");
        if (!file) {
            perror("File opening failed");
            exit(EXIT_FAILURE);
        }

        // Read the file content from client and write it to the file
        char received_text[MAX_BUFFER_SIZE] = {0}; // Buffer to hold the received content
        while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            fwrite(buffer, sizeof(char), read_size, file);
            strncat(received_text, buffer, read_size); // Append received data to the received_text buffer
        }

        if (read_size == 0) {
            printf("File received successfully\n");

            // Trim the received text (remove leading/trailing spaces and newlines)
            trim(received_text);

            // Display the received text on the console
            printf("\nReceived text: '%s'\n", received_text); // Display the received text

            // Check if the received text is a palindrome
            if (is_palindrome(received_text)) {
                fprintf(file, "\nThe content is a Palindrome.\n");
                printf("The content is a Palindrome.\n");
            } else {
                fprintf(file, "\nThe content is not a Palindrome.\n");
                printf("The content is not a palindrome.\n");
            }
        } else if (read_size < 0) {
            perror("Recv failed");
        }
        reverse(received_text);
        fprintf(file,"\nReversed String : %s\n",str2);

        fclose(file);
        printf("File saved as 'output.txt'\n");

        // Close the connection
        close(client_socket);
    }

    close(server_socket);
    return 0;
}







