NetScope // Packet Analysis Engine

NetScope is a full-stack network telemetry platform designed to bridge the gap between kernel-level packet inspection and modern web-based monitoring. It captures raw network traffic, performs real-time protocol decoding, and executes behavioral security analysis.

 Key Technical Features

Kernel-Level Sniffing: Implements raw Linux sockets (AF_PACKET) in C to capture traffic with sub-millisecond latency.

Algorithmic Analytics: Utilizes a custom O(1) Hash Map (DJB2) for memory-efficient tracking of connection frequency.

Security IDS: Real-time Intrusion Detection System that triggers alerts upon detecting reconnaissance-style port scans.

Real-Time Data Bridge: Streams telemetry to a React frontend via Server-Sent Events (SSE) through a Flask-based backend.

🛠 Prerequisites

OS: Linux (Ubuntu 22.04+ recommended)

Compilers: gcc

Runtime: Python 3.x, Node.js (v18+) & npm

📋 How to Run

1. Build the C Engine

Navigate to the netscope-backend directory and compile the sniffer binary:

cd netscope-backend
gcc sniffer.c -o sniffer


2. Start the Backend

Install the necessary Python dependencies and initiate the stream server:

# It is recommended to use a virtual environment
pip install flask
# Root privileges are required for raw socket access
sudo python3 server.py


3. Start the Frontend

In a separate terminal window, navigate to the netscope-frontend directory, install the project dependencies, and launch the development server:

cd ../netscope-frontend
npm install
npm run dev


4. Access the Dashboard

Open your browser and navigate to the address displayed in your terminal (typically http://localhost:5173).

🏗 Project Architecture

The architecture flows from the C-based raw socket sniffer -> Flask (SSE) -> React Frontend.

🔮 Future Roadmap

ML Integration: Implementation of Scikit-learn models (e.g., Isolation Forests) for automated anomaly detection.

Forensics: Implementation of PCAP file exporting for persistent packet forensics.
