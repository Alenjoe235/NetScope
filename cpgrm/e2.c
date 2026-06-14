#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h> //which gives us the struct ethhdr definition
#include <arpa/inet.h>
#include <linux/if_ether.h>     

void print_mac(unsigned char *mac){
    for(int i = 0; i < 6; i++){
        printf("%02x ", mac[i]);
    }
    printf("\n");
}

int main(){
    unsigned char buffer[65536];
    int captured_packets = 0;
    
    //creating raw socket to capture all packets
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE); 
    }
    printf("Raw socket created, capturing packets for all seperatly \n");

    while(1){
        //capture IPv4 packets
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0) continue; //if packet capture fails, skip to the next iteration

        //the stencil for the Ethernet header is 14 bytes, and the EtherType for IPv4 is 0x0800
        struct ethhdr * eth = (struct ethhdr*) buffer;

        unsigned short protocol = ntohs(eth->h_proto);
        if(protocol == ETH_P_IP){
            captured_packets++;
            printf("Captured IPv4 packet: %d\n", captured_packets);
            printf("Source MAC: ");
            print_mac(eth->h_source);
            printf("Destination MAC: ");
            print_mac(eth->h_dest);       
           
        }
        else if(protocol == ETH_P_ARP){
            captured_packets++;
            printf("Captured ARP packet: %d\n", captured_packets);
            printf("Source MAC: ");
            print_mac(eth->h_source);
            printf("Destination MAC: ");
            print_mac(eth->h_dest);
        }else if(protocol == ETH_P_IPV6){
            captured_packets++;
            printf("Captured IPv6 packet: %d\n", captured_packets);
            printf("Source MAC: ");
            print_mac(eth->h_source);
            printf("Destination MAC: ");
            print_mac(eth->h_dest);
        }

        
    
        
    }
    close(raw_socket);
        return 0;
}


/*
In standard networking, Layer 2 (The Data Link Layer) almost universally uses the Ethernet II frame format. 
The Ethernet II header is strictly standardized to be exactly 14 bytes long, 
no matter what kind of data is inside the packet.
  Here is exactly how those 14 bytes are spent:
  Bytes 0 to 5 (6 bytes): Destination MAC Address.
  Bytes 6 to 11 (6 bytes): Source MAC Address.
  Bytes 12 to 13 (2 bytes): EtherType
   (The protocol code, like 0x0800 for IPv4).Total = 14 Bytes.When we cast (struct ethhdr *)buffer or 
   loop for (int i = 0; i < 14; i++), we are simply drawing a box around the first 14 bytes of the payload
    because we know with absolute certainty that this is the Ethernet header. Everything from 
    byte 15 onward belongs to Layer 3 (the IP Header) and beyond.
*/