#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h> // Gives us struct tcphdr
#include <netinet/udp.h> // Gives us struct udphdr

int main(){
    unsigned char buffer[65536];
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE); 
    }
    printf("Raw socket created, capturing packets for all seperatly \n");

    while(1){
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0) continue; //if packet capture fails, skip to the

        //ensuring the packet is an IPV4 packet
        struct ethhdr * eth = (struct ethhdr*) buffer;
        unsigned short protocol = ntohs(eth->h_proto);

        if(protocol == ETH_P_IP){
            struct iphdr * ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            int ip_header_length = ip->ihl * 4; //the IHL field gives the number of 32-bit words in the header, so we multiply by 4 to get the length in bytes
            int total_length = ntohs(ip->tot_len); //the total length field gives the

            struct in_addr sour_ip, dest_ip;
            sour_ip.s_addr = ip->saddr;
            dest_ip.s_addr = ip->daddr;

        //the critical part is to ensure that the packet is a TCP or UDP packet, as we will be extracting the source and destination ports from the TCP or UDP header, which starts after the IP header
        unsigned char *transport_buffer = buffer + sizeof(struct ethhdr) + ip_header_length; //the transport header starts after the Ethernet and IP headers
        //we are checking whether the protocol is tcp / udp
        if(ip->protocol == IPPROTO_TCP){ //6
            struct tcphdr *tcp = (struct tcphdr*) transport_buffer;

            //ntohs is used to convert the port numbers from network byte order to host byte order
            printf("[TCP] %s : %d -> %s :  %d\n" ,inet_ntoa(sour_ip), ntohs(tcp->source), inet_ntoa(dest_ip), ntohs(tcp->dest));

            printf("TCP Flags: ");
            if(tcp->urg) printf("URG ");
            if(tcp->ack) printf("ACK ");
            if(tcp->syn) printf("SYN ");
            if(tcp->fin) printf("FIN ");
            if(tcp->rst) printf("RST ");
            if(tcp->psh) printf("PSH "); 
            printf("\n");

        }else if(ip->protocol == IPPROTO_UDP){ //17
            struct udphdr *udp = (struct udphdr*) transport_buffer;
            printf("[UDP] %s : %d -> %s :  %d\n" ,inet_ntoa(sour_ip), ntohs(udp->source), inet_ntoa(dest_ip), ntohs(udp->dest));
        }
    }


    }
    close(raw_socket);
    return 0;


}