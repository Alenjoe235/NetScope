#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h> //which gives us the struct ethhdr definition
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h> //which gives us the struct iphdr definition

int main(){
    unsigned char buffer[65536];
    int packets_captured = 0;
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE); 
    }
    printf("Raw socket created, capturing packets for all seperatly \n");

    while(1){
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0 ,NULL, NULL);
        if((data_size) < 0) continue; //if packet capture fails, skip to the next iteration

        struct ethhdr *eth = (struct ethhdr*) buffer;
        unsigned short protocol = ntohs(eth->h_proto);
        if(protocol == ETH_P_IP){   

            struct iphdr * ip = (struct iphdr*)(buffer + sizeof(struct ethhdr)); //the IP header starts after the Ethernet header
            packets_captured++;
            printf("Captured IPv4 packet: %d\n", packets_captured);
        //extracting data
        int ip_header_length = ip->ihl * 4; //the IHL field gives the number of 32-bit words in the header, so we multiply by 4 to get the length in bytes
        int total_length = ntohs(ip->tot_len); //the total length field gives the length of the entire IP packet, including the header and data
        int data_length = total_length - ip_header_length; //the length of the data is
        int ttl = ip->ttl ; //the TTL field gives the time to live of the packet, which is decremented by each router it passes through
        int next_protocol = ip->protocol; //the protocol field gives the protocol of the data (e.g., TCP, UDP, ICMP)

        //converting the source and destination IP addresses from binary to human-readable format
        struct in_addr source_ip, dest_ip;
        source_ip.s_addr = ip->saddr;
        dest_ip.s_addr = ip->daddr;

        if(next_protocol == IPPROTO_TCP){ //6 is the protocol number for TCP
            printf("Protocol: TCP\n");
        }else if(next_protocol == IPPROTO_UDP){ //17 is the protocol number for UDP
            printf("Protocol: UDP\n");
        }else if(next_protocol == IPPROTO_ICMP){   //1 is the protocol number for ICMP
            printf("Protocol: ICMP\n");
        }else{
            printf("Protocol: Other\n");
      }

      printf("Source IP: %s\n", inet_ntoa(source_ip));
      printf("Destination IP: %s\n", inet_ntoa(dest_ip));
      printf("TTL: %d\n", ttl);
      printf("Data Length: %d bytes\n", data_length);
    }

    }
  close(raw_socket);
  return 0;
}