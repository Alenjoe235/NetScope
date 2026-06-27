# NetScope // Packet Analysis Engine

**NetScope** is a full-stack network telemetry platform designed to bridge the gap between **kernel-level packet inspection** and modern **web-based monitoring**. It captures raw network traffic, performs real-time protocol decoding, and executes behavioral security analysis.

---

## Key Technical Features

### Kernel-Level Sniffing
Implements raw Linux sockets (`AF_PACKET`) in **C** to capture network traffic with **sub-millisecond latency**.

### Algorithmic Analytics
Utilizes a custom **O(1) Hash Map** using the **DJB2 hashing algorithm** for memory-efficient tracking of connection frequency.

### Security IDS
Includes a real-time **Intrusion Detection System (IDS)** that triggers alerts upon detecting **reconnaissance-style port scans**.

### Real-Time Data Bridge
Streams telemetry data to a **React frontend** via **Server-Sent Events (SSE)** through a **Flask-based backend**.

---

## Tech Stack

- **Low-Level Engine:** C  
- **Backend API:** Python + Flask  
- **Frontend:** React.js  
- **Streaming Protocol:** Server-Sent Events (SSE)  
- **Operating System:** Linux  
- **Networking:** Raw Sockets (`AF_PACKET`)  

---

## Prerequisites

Before running the project, ensure you have:

- **Operating System:** Linux (*Ubuntu 22.04+ recommended*)
- **Compiler:** GCC  
- **Runtime:** Python 3.x  
- **Frontend Runtime:** Node.js (v18+) and npm  

---

## How to Run

### 1. Build the C Engine

Navigate to the backend directory and compile the packet sniffer binary.

```bash
cd netscope-backend
gcc sniffer.c -o sniffer
```

---

### 2. Start the Backend Server

Install the required Python dependencies and start the Flask server.

```bash
# Recommended: Use a virtual environment
pip install flask

# Root privileges required for raw socket access
sudo python3 server.py
```

---

### 3. Start the Frontend

Open a separate terminal, navigate to the frontend directory, install dependencies, and run the React development server.

```bash
cd ../netscope-frontend
npm install
npm run dev
```

---

### 4. Access the Dashboard

Open your browser and visit:

```text
http://localhost:5173
```

*(Or the local address shown in your terminal.)*

---

## Project Architecture

The system architecture follows this pipeline:

```text
Raw Network Packets
        │
        ▼
C-based Packet Sniffer (AF_PACKET)
        │
        ▼
Protocol Parsing & Security Analysis
        │
        ▼
Flask Backend Server
        │
   Server-Sent Events (SSE)
        │
        ▼
React Frontend Dashboard
        │
        ▼
Real-Time Monitoring & Alerts
```

---

## Core Functionalities

- Real-time packet capture at kernel level  
- Protocol decoding and packet inspection  
- Connection frequency monitoring using custom hash maps  
- Port scan detection and intrusion alerts  
- Live telemetry streaming to web dashboard  
- Low-latency network monitoring pipeline  

---

## Future Roadmap

### ML Integration
Implement anomaly detection models using **Scikit-learn**, such as:

- Isolation Forest  
- One-Class SVM  
- Unsupervised traffic anomaly detection  

### Packet Forensics
Add support for **PCAP file exporting** for persistent packet analysis and digital forensics.

### Advanced Detection
Expand intrusion detection capabilities to detect:

- SYN Flood attacks  
- ARP Spoofing  
- DNS Tunneling  
- Suspicious traffic anomalies  

---

## Learning Outcomes

This project demonstrates understanding of:

- Low-level systems programming in **C**
- Linux networking and **raw socket programming**
- Real-time backend communication with **SSE**
- Full-stack development using **Flask + React**
- Network security concepts and intrusion detection
- Performance-oriented data structures and hashing algorithms

---

## License

This project is developed for educational and research purposes.
