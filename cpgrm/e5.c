//application layer
/*Right now, you are extracting the Port Numbers (Layer 4). The next step is to look inside the TCP or UDP envelope to read the actual message. 
This is where you catch things like HTTP GET requests or DNS queries*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

int main(){

    unsigned char buffer[65536];
    
    // Using AF_PACKET and ETH_P_ALL to capture EVERYTHING at Layer 2 (Ethernet)
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE); 
    }
    printf("Raw socket created, capturing packets for all seperatly \n");

    while(1){
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0) continue; //if packets capture fails, skip to the next iteration
        
        //ensuring the packet is an IPV4 packet        
        struct ethhdr *eth = (struct ethhdr*) buffer;
        unsigned short protocol = ntohs(eth->h_proto);
        
        if(protocol == ETH_P_IP){
            // Skipping the 14-byte Ethernet header to get to the IP header
            struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            
            int ip_header_length = ip->ihl * 4; //the IHL field gives the number of 32-bit words in the header, so we multiply by 4 to get the length in bytes
            
            unsigned char *transport_buffer = buffer + sizeof(struct ethhdr) + ip_header_length; //the transport header starts after the Ethernet and IP headers
            
            if(ip->protocol == IPPROTO_TCP){ //6
                struct tcphdr *tcp = (struct tcphdr*) transport_buffer;
                int tcp_header_length = tcp->doff * 4; //the DOFF field , we multiply by 4 to get the length in bytes

                unsigned char *payload_buffer = transport_buffer + tcp_header_length; //the payload starts after the TCP header
                int payload_length  = ntohs(ip->tot_len) - ip_header_length - tcp_header_length; //the length of the payload is the total length of the IP packet minus the lengths of the IP and TCP headers
                
                if(payload_length > 0){
                    if(strncmp((char *)payload_buffer, "GET ", 4) == 0 ){
                        
                        // Split printfs to avoid inet_ntoa static buffer overwriting itself
                        printf("\nHTTP GET request captured: ");
                        printf("%s : %d -> ", inet_ntoa(*(struct in_addr *)&ip->saddr), ntohs(tcp->source));
                        printf("%s : %d\n", inet_ntoa(*(struct in_addr *)&ip->daddr), ntohs(tcp->dest));
                        
                        printf("Payload: %.*s \n", 50, payload_buffer);
                   }
                }   
            }
            else if(ip->protocol == IPPROTO_UDP){ //17
                struct udphdr *udp = (struct udphdr*) transport_buffer;
                unsigned char *payload_buffer = transport_buffer + sizeof(struct udphdr); //the payload starts after the UDP header
                //it is always 8 bytes long, so we can directly add the size of the UDP header to get the start of the payload
                
                if(ntohs(udp->source) == 53 || ntohs(udp->dest) == 53){ //checking if the source or destination port is 53, which is the port used for DNS queries
                    
                    // Split printfs to avoid inet_ntoa static buffer overwriting itself
                    printf("DNS query captured: ");
                    printf("%s : %d -> ", inet_ntoa(*(struct in_addr *)&ip->saddr), ntohs(udp->source));
                    printf("%s : %d\n", inet_ntoa(*(struct in_addr *)&ip->daddr), ntohs(udp->dest));
                    
                   // printf("Payload %.*s \n",50, payload_buffer); 
                   // we are not printing the payload of the DNS query because it is in a binary format and would not be human-readable.
                   // To properly parse and display the contents of a DNS query, you would need to implement a DNS parser that can interpret the various fields and sections of the DNS message. 
                   // This involves understanding the structure of DNS messages, including the header, question section, answer section, authority section, and additional section. Each of these sections contains specific information that can be extracted and displayed in a human-readable format.
                }
            }
        }
    }

    close(raw_socket);
    return 0;
}