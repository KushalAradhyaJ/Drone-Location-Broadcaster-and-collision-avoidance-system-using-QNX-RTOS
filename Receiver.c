#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

#define PORT 5555
#define COLLISION_THRESHOLD 100.0 // Danger distance in meters

// Ensure this struct matches your Transmitter exactly
typedef struct {
    uint32_t drone_id;
    double lat;
    double lon;
    double alt;
    double vx;
    double vy;
    uint64_t timestamp;
} adsb_packet_t;

// Global variables to store the state of our two drones
adsb_packet_t drone101, drone102;
int d101_active = 0;
int d102_active = 0;

// Simple Haversine-style approximation for distance in meters
double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    double dx = (lon2 - lon1) * 111320.0 * cos(lat1 * M_PI / 180.0);
    double dy = (lat2 - lat1) * 110540.0;
    return sqrt(dx * dx + dy * dy);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    adsb_packet_t packet;
    socklen_t len = sizeof(cliaddr);

    // 1. Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 2. Bind socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("--- ADSB Collision Receiver Active (Monitoring Drones 101 & 102) ---\n");

    while (1) {
        int n = recvfrom(sockfd, &packet, sizeof(adsb_packet_t), 0, (struct sockaddr *)&cliaddr, &len);

        if (n > 0) {
            // Update the stored position for the specific drone ID
            if (packet.drone_id == 101) {
                drone101 = packet;
                d101_active = 1;
            } else if (packet.drone_id == 102) {
                drone102 = packet;
                d102_active = 1;
            }

            // Only perform collision check if we have seen BOTH drones at least once
            if (d101_active && d102_active) {
                double distance = calculate_distance(drone101.lat, drone101.lon, drone102.lat, drone102.lon);

                printf("[IDs: 101 & 102] Separation: %.2f m | ", distance);

                if (distance < COLLISION_THRESHOLD) {
                    printf("!!! WARNING: POTENTIAL COLLISION !!!\n");
                } else {
                    printf("Status: Clear\n");
                }
            } else {
                printf("Waiting for data from both drones... (Current: %s)\n",
                        packet.drone_id == 101 ? "101 active" : "102 active");
            }
        }
    }

    close(sockfd);
    return 0;
}
