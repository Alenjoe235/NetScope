// src/mockData.js
export const initialPackets = [
  { id: 1, timestamp: "22:14:01", source: "192.168.1.5", destination: "8.8.8.8", protocol: "DNS (UDP)", size: 64, info: "Standard query A google.com" },
  { id: 2, timestamp: "22:14:02", source: "192.168.1.5", destination: "142.250.190.46", protocol: "HTTPS (TCP)", size: 1514, info: "[SYN] Seq=0 Win=64240" },
  { id: 3, timestamp: "22:14:03", source: "142.250.190.46", destination: "192.168.1.5", protocol: "HTTPS (TCP)", size: 1460, info: "[SYN, ACK] Seq=0 Ack=1" },
  { id: 4, timestamp: "22:14:05", source: "192.168.1.1", destination: "192.168.1.5", protocol: "ICMP", size: 98, info: "Echo (ping) reply id=0x0001" },
];