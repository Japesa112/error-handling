
#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RETRANSMISSIONS 3
#define ERROR_PROBABILITY 0.1 // 10% chance of error

static struct unicast_conn uc;
static int retransmissions;

// Function to simulate bit errors
int introduce_bit_error(int message) {
    if ((rand() / (double)RAND_MAX) < ERROR_PROBABILITY) {
        // Introduce an error by flipping the least significant bit
        return message ^ 0x01;
    }
    return message;
}

// Unicast receive callback
static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) {
    int received_message = *(int *)packetbuf_dataptr();
    printf("Received message: %d from %d.%d\n", received_message, from->u8[0], from->u8[1]);

    // Simulate error detection (for simplicity, assume even number is valid)
    if (received_message % 2 == 0) {
        printf("Message received correctly.\n");
        retransmissions = 0; // Reset retransmissions
    } else {
        printf("Error detected! Retransmitting...\n");
        if (retransmissions < MAX_RETRANSMISSIONS) {
            retransmissions++;
            packetbuf_copyfrom(&received_message, sizeof(received_message));
            unicast_send(&uc, from);
        } else {
            printf("Max retransmissions reached. Giving up.\n");
        }
    }
}

// Unicast send callback
static void sent_uc(struct unicast_conn *c, int status, int num_tx) {
    printf("Message sent, status: %d, retransmissions: %d\n", status, num_tx);
}

static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};

PROCESS(error_handling_process, "Error Handling Process");
AUTOSTART_PROCESSES(&error_handling_process);

PROCESS_THREAD(error_handling_process, ev, data) {
    static struct etimer timer;
    static int message;
    linkaddr_t dest;

    PROCESS_BEGIN();
    unicast_open(&uc, 146, &unicast_callbacks);
    srand(clock_time()); // Seed for random number generation

    while (1) {
        etimer_set(&timer, CLOCK_SECOND * 2);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        // Prepare message
        message = rand() % 100; // Random message
        message = introduce_bit_error(message); // Introduce possible error

        // Set destination (broadcasting to self for testing)
        linkaddr_copy(&dest, &linkaddr_node_addr);

        packetbuf_copyfrom(&message, sizeof(message));
        unicast_send(&uc, &dest); // Send message
        printf("Sent message: %d\n", message);
    }

    PROCESS_END();
}
