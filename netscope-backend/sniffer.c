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
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(raw_socket < 0) return 1;

    while(1){
        int data_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0) continue; 
               
        struct ethhdr *eth = (struct ethhdr*) buffer;
        unsigned short protocol = ntohs(eth->h_proto);
        
        if(protocol == ETH_P_IP){
            struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            int ip_header_length = ip->ihl * 4; 
            unsigned char *transport_buffer = buffer + sizeof(struct ethhdr) + ip_header_length; 
            
            char src_ip[16], dst_ip[16];
            // Safely copy IPs to local buffers to avoid inet_ntoa overwrite issues
            strcpy(src_ip, inet_ntoa(*(struct in_addr *)&ip->saddr));
            strcpy(dst_ip, inet_ntoa(*(struct in_addr *)&ip->daddr));

            if(ip->protocol == IPPROTO_TCP){ 
                struct tcphdr *tcp = (struct tcphdr*) transport_buffer;
                int tcp_header_length = tcp->doff * 4; 
                unsigned char *payload_buffer = transport_buffer + tcp_header_length; 
                int payload_length = ntohs(ip->tot_len) - ip_header_length - tcp_header_length; 
                
                char app_proto[20] = "TCP_OTHER";
                
                if(payload_length > 0 && strncmp((char *)payload_buffer, "GET ", 4) == 0){
                    strcpy(app_proto, "HTTP_GET");
                }
                
                // Print as a strict JSON object
                printf("{\"layer4\": \"TCP\", \"src_ip\": \"%s\", \"src_port\": %d, \"dst_ip\": \"%s\", \"dst_port\": %d, \"app\": \"%s\", \"payload_bytes\": %d}\n", 
                       src_ip, ntohs(tcp->source), dst_ip, ntohs(tcp->dest), app_proto, payload_length);
                       
                // CRITICAL: Flush the standard output buffer immediately
                fflush(stdout);
            }
            else if(ip->protocol == IPPROTO_UDP){ 
                struct udphdr *udp = (struct udphdr*) transport_buffer;
                
                char app_proto[20] = "UDP_OTHER";
                if(ntohs(udp->source) == 53 || ntohs(udp->dest) == 53){ 
                    strcpy(app_proto, "DNS");
                }

                // Print as a strict JSON object
                printf("{\"layer4\": \"UDP\", \"src_ip\": \"%s\", \"src_port\": %d, \"dst_ip\": \"%s\", \"dst_port\": %d, \"app\": \"%s\", \"payload_bytes\": %d}\n", 
                       src_ip, ntohs(udp->source), dst_ip, ntohs(udp->dest), app_proto, ntohs(udp->len) - 8);
                       
                // CRITICAL: Flush the standard output buffer immediately
                fflush(stdout);
            }
        }
    }
    close(raw_socket);
    return 0;
}