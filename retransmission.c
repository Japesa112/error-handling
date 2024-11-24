#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1" // Change this to the server's IP in Cooja
#define PACKET_SIZE 1024
#define MAX_RETRIES 5
#define ERROR_PROBABILITY 0.1 // 10% chance of bit error

void introduce_bit_error(char *packet) {
    for (int i = 0; i < PACKET_SIZE; i++) {
        if ((rand() / (float)RAND_MAX) < ERROR_PROBABILITY) {
            packet[i] ^= 1; // Flip a random bit
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char packet[PACKET_SIZE];
    char ack[3]; // Acknowledgment buffer
    int retries;

    // Initialize random number generator
    srand(time(NULL));

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        memset(packet, 0, PACKET_SIZE);
        snprintf(packet, PACKET_SIZE, "Hello from mote!");

        introduce_bit_error(packet); // Simulate bit errors

        retries = 0;
        while (retries < MAX_RETRIES) {
            // Send packet to the server
            sendto(sockfd, packet, PACKET_SIZE, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

            // Wait for acknowledgment
            socklen_t addr_len = sizeof(server_addr);
            int n = recvfrom(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&server_addr, &addr_len);
            if (n > 0 && strcmp(ack, "ACK") == 0) {
                printf("Packet sent successfully and acknowledged.\n");
                break; // Exit retry loop on success
            }

            printf("No ACK received. Retrying...\n");
            retries++;
            sleep(1); // Wait before retrying
        }

        if (retries == MAX_RETRIES) {
            printf("Failed to send packet after %d retries.\n", MAX_RETRIES);
        }

        sleep(2); // Wait before sending the next packet
    }

    close(sockfd);
    return 0;
}
