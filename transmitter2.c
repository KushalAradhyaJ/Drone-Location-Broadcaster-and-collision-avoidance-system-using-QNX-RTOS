#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

// 1. The Data Structure (Must match the Receiver exactly)
typedef struct {
    uint32_t drone_id;
    double lat;
    double lon;
    double alt;
    double vx;
    double vy;
    uint64_t timestamp;
} adsb_packet_t;

#define PORT 5555

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    int broadcastEnable = 1;

    printf("--- QNX ADS-B Transmitter Initializing ---\n");

    // 2. Create UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // 3. Enable Broadcasting (Crucial for ADS-B behavior)
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Setsockopt SO_BROADCAST failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // 4. Setup Destination Address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    // Use 255.255.255.255 to hit everything on the VMware virtual network
    servaddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    // 5. Initialize Simulated Drone State
    adsb_packet_t my_drone;
    my_drone.drone_id = 102;
    my_drone.lat = 34.0600;
    my_drone.lon = -118.2400;
    my_drone.alt = 150.0;      // 150 meters
    my_drone.vx = -0.0001; // Moving South
    my_drone.vy = -0.0001; // Moving West
    my_drone.timestamp = 0;

    printf("Sending ADS-B Out signals on Port %d...\n", PORT);

    while(1) {
        // Simulate movement: increment lat/lon slightly each second
        my_drone.lat += my_drone.vx;
        my_drone.lon += my_drone.vy;
        my_drone.timestamp++;

        // 6. Send the Packet
        ssize_t sent_bytes = sendto(sockfd, &my_drone, sizeof(adsb_packet_t), 0,
                                    (const struct sockaddr *) &servaddr, sizeof(servaddr));

        if (sent_bytes < 0) {
            perror("sendto failed");
        } else {
            printf("[ID:%d] Sent Pos: %.4f, %.4f | Alt: %.1f m\n",
                    my_drone.drone_id, my_drone.lat, my_drone.lon, my_drone.alt);
        }

        // QNX sleep (1 second)
        sleep(1);
    }

    close(sockfd);
    return 0;
}
