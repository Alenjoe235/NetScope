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
#include <ctype.h>

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

            char layer4[10] = "", app[20] = "OTHER";
            char http_method[16] = "", dns_query[256] = "", smtp_cmd[32] = "";
            int src_port = 0, dst_port = 0, payload_bytes = 0;

            if(ip->protocol == IPPROTO_TCP){ 
                strcpy(layer4, "TCP");
            struct tcphdr *tcp = (struct tcphdr *)transport_buffer;
            src_port = ntohs(tcp->source);
            dst_port = ntohs(tcp->dest);

            // Ignore Flask to React SSE traffic to prevent infinite loops
            if (src_port == 5000 || dst_port == 5000 || src_port == 5173 || dst_port == 5173) {
                continue; 
            }
            // ===
            int tcp_hdr_len = tcp->doff * 4;
            unsigned char *payload = transport_buffer + tcp_hdr_len;
            payload_bytes = ntohs(ip->tot_len) - ip_header_length - tcp_hdr_len;



            if (payload_bytes > 0) {
                // 1. HTTP Decoder
                if (src_port == 80 || dst_port == 80 || src_port == 8080 || dst_port == 8080) {
                    strcpy(app, "HTTP");
                    if (strncmp((char *)payload, "GET ", 4) == 0) strcpy(http_method, "GET");
                    else if (strncmp((char *)payload, "POST ", 5) == 0) strcpy(http_method, "POST");
                    else if (strncmp((char *)payload, "HTTP/", 5) == 0) strcpy(http_method, "HTTP_RES");
                }
                // 2. SMTP Decoder 
                else if (src_port == 25 || dst_port == 25 || src_port == 587 || dst_port == 587) {
                    strcpy(app, "SMTP");
                    if (strncmp((char *)payload, "EHLO ", 5) == 0) strcpy(smtp_cmd, "EHLO");
                    else if (strncmp((char *)payload, "HELO ", 5) == 0) strcpy(smtp_cmd, "HELO");
                    else if (strncmp((char *)payload, "MAIL FROM:", 10) == 0) strcpy(smtp_cmd, "MAIL FROM");
                    else if (strncmp((char *)payload, "RCPT TO:", 8) == 0) strcpy(smtp_cmd, "RCPT TO");
                } else {
                    strcpy(app, "TCP_OTHER");
                }
            } else {
                strcpy(app, "TCP_CTRL");
            }
            }
            else if(ip->protocol == IPPROTO_UDP){ 
          strcpy(layer4, "UDP");
            struct udphdr *udp = (struct udphdr *)transport_buffer;
            src_port = ntohs(udp->source);
            dst_port = ntohs(udp->dest);
            payload_bytes = ntohs(udp->len) - 8;
            unsigned char *payload = transport_buffer + 8;

            // 3. DNS Decoder 
            if (src_port == 53 || dst_port == 53) {
                strcpy(app, "DNS");
                // DNS header is 12 bytes. Question section starts at offset 12.
                if (payload_bytes > 12) {
                    int offset = 12;
                    int i = 0;
                    while (offset < payload_bytes && payload[offset] != 0 && i < 254) {
                        int len = payload[offset];
                        if ((len & 0xC0) == 0xC0) break; // Stop at DNS pointers
                        offset++;
                        for (int j = 0; j < len && offset < payload_bytes; j++) {
                            if(isprint(payload[offset])) {
                                dns_query[i++] = payload[offset];
                            }
                            offset++;
                        }
                        dns_query[i++] = '.';
                    }
                    if (i > 0) dns_query[i - 1] = '\0'; // Remove trailing dot
                }
            } else {
                strcpy(app, "UDP_OTHER");
            }
        }
        if (strlen(layer4) > 0) {
            // Updated JSON print with the new Week 4 fields
            printf("{\"layer4\": \"%s\", \"src_ip\": \"%s\", \"src_port\": %d, \"dst_ip\": \"%s\", \"dst_port\": %d, \"app\": \"%s\", \"payload_bytes\": %d, \"http_method\": \"%s\", \"dns_query\": \"%s\", \"smtp_cmd\": \"%s\"}\n", 
                   layer4, src_ip, src_port, dst_ip, dst_port, app, payload_bytes, http_method, dns_query, smtp_cmd);
            fflush(stdout);
        }
            }
        }
    
    close(raw_socket);
    return 0;
}