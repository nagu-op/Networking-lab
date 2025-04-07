// udp_server.c
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#define PORT 4000

int main() {
    int sockfd, n;
    char buffer[1024], filename[100];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t addr_len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("Server is running and waiting for filename...\n");

    // Receive filename from client
    n = recvfrom(sockfd, filename, sizeof(filename)-1, 0, (struct sockaddr*)&cliaddr, &addr_len);
    filename[n] = '\0';

    printf("Client requested file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File not found");
        strcpy(buffer, "File not found on server.");
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&cliaddr, addr_len);
        close(sockfd);
        return 1;
    }

    // Read and send content
    while (fgets(buffer, sizeof(buffer), file)) {
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&cliaddr, addr_len);
    }

    // Send EOF message
    strcpy(buffer, "EOF");
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&cliaddr, addr_len);

    fclose(file);
    printf("File sent successfully.\n");
    close(sockfd);
    return 0;
}
