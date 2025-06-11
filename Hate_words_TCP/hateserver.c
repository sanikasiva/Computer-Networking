#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>  // For tolower()

#define PORT 6505
#define MAX_BUFFER_SIZE 1024

// List of hate words (can be expanded)
const char *HATE_WORDS[] = {
    "hate", "violence", "anger", "racist", "terror","angry","kill"
};
const int NUM_HATE_WORDS = 7;

// Helper function to convert a string to lowercase
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int check_for_hate_words(const char *text) {
    char lower_text[MAX_BUFFER_SIZE];
    strcpy(lower_text, text);  // Make a copy of the text for case-insensitive comparison

    // Convert the entire text to lowercase
    to_lowercase(lower_text);

    // Check if any of the hate words are present in the text
    for (int i = 0; i < NUM_HATE_WORDS; i++) {
        if (strstr(lower_text, HATE_WORDS[i]) != NULL) {
            return 0;  // Hate word found
        }
    }
    return 1;  // No hate words found
}

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    char file_name[MAX_BUFFER_SIZE];
    FILE *file;
    char text[MAX_BUFFER_SIZE];

    // Receive the file name from the client
    memset(file_name, 0, sizeof(file_name));
    int len = recv(client_socket, file_name, sizeof(file_name) - 1, 0);
    if (len <= 0) {
        perror("Error receiving file name");
        close(client_socket);
        return;
    }

    // Open the file sent by the client
    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("File not found");
        send(client_socket, "Error: File not found", 21, 0);
        close(client_socket);
        return;
    }

    // Read the file content
    fread(text, sizeof(char), MAX_BUFFER_SIZE, file);
    fclose(file);

    printf("Received text: %s\n", text);

    // Check for hate words in the text
    if (check_for_hate_words(text)) {
        send(client_socket, "Approved", 8, 0);
    } else {
        send(client_socket, "Rejected", 8, 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection from client
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) != -1) {
        printf("Client connected.\n");

        // Handle the client
        handle_client(client_socket);
    }

    if (client_socket == -1) {
        perror("Accept failed");
    }

    close(server_socket);
    return 0;
}


/*
SERVER OUTPUT

cs-db-23@CS-DB-23:~/Sanika/Practice$ ./hateserver
Server listening on port 6505...
Client connected.
Received text: The man threatened to kill his opponent.
*/
