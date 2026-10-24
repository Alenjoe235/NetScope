//this program is for capturing packets using raw sockets in C. It creates a raw socket that listens to all packets on the network interface and 
//prints the first 14 bytes of each captured packet, which corresponds to the Ethernet header. The program runs indefinitely until manually terminated, and it counts the number of packets captured.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

int main(){
    int raw_socket;
    unsigned char buffer[65536];
    int packets_captured = 0;

    //creating socket listening to all packets
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket <  0){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Raw socket capturing packets");

    while(1){
        //capture packets
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0){
            perror("packet capture failed");
            exit(EXIT_FAILURE);
        }
        packets_captured++;
        printf("Packet captured: %d\n", packets_captured);

        for(int i = 0; i < 14;i++){
            //printing the first 14 bytes of the packet (Ethernet header)
            printf("%02x ", buffer[i]);

        }


    }

    close(raw_socket);

}

    
