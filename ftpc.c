// udp_client.c
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#define PORT 4000

int main() {
    int sockfd, n;
    char filename[100], new_filename[100], buffer[1024];
    struct sockaddr_in servaddr;
    socklen_t addr_len = sizeof(servaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    printf("Enter existing filename to request: ");
    scanf("%s", filename);

    printf("Enter new filename to save received content: ");
    scanf("%s", new_filename);

    // Send filename to server
    sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr*)&servaddr, addr_len);

    FILE *new_file = fopen(new_filename, "w");
    if (!new_file) {
        perror("Cannot open new file");
        close(sockfd);
        return 1;
    }

    printf("Receiving file content...\n");

    while (1) {
        n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&servaddr, &addr_len);
        buffer[n] = '\0';

        if (strcmp(buffer, "EOF") == 0) {
            break;
        }

        printf("%s", buffer);
        fprintf(new_file, "%s", buffer);
    }

    printf("\nFile received and saved successfully as %s\n", new_filename);
    fclose(new_file);
    close(sockfd);
    return 0;
}
