from flask import Flask, Response
from flask_cors import CORS
import subprocess
import json
import time
import random

app = Flask(__name__)
CORS(app) 

def generate_real_packets():
    # 1. Launch the compiled C executable
    # It inherits root privileges from the Python script
    process = subprocess.Popen(
        ['./sniffer'], 
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    try:
        # 2. Read the C program's output line by line forever
        for line in iter(process.stdout.readline, ''):
            if not line:
                continue
            
            try:
                # 3. Parse the JSON string printed by C
                c_data = json.loads(line.strip())
                
                # 4. Map the C format to the format React expects
                react_packet = {
                    "id": ''.join(random.choices('abcdefghijklmnopqrstuvwxyz0123456789', k=9)),
                    "timestamp": time.strftime("%H:%M:%S"),
                    "source": c_data.get("src_ip", "Unknown"),
                    "destination": c_data.get("dst_ip", "Unknown"),
                    # If it's standard TCP/UDP, use that. If DNS or HTTP, use the app protocol for cooler badges!
                    "protocol": c_data.get("app") if c_data.get("app") not in ["TCP_OTHER", "UDP_OTHER"] else c_data.get("layer4"),
                    "size": c_data.get("payload_bytes", 0),
                    "info": f"Src Port: {c_data.get('src_port')} -> Dst Port: {c_data.get('dst_port')}"
                }
                
                # 5. Send to React!
                yield f"data: {json.dumps(react_packet)}\n\n"
                
            except json.JSONDecodeError:
                # If C prints a non-JSON error or debug message, ignore it
                continue
    finally:
        # If the browser disconnects, kill the C process to prevent memory leaks
        process.terminate()

@app.route('/stream')
def stream():
    return Response(generate_real_packets(), mimetype='text/event-stream')

if __name__ == '__main__':
    print("Starting LIVE NetScope Backend on http://localhost:5000...")
    app.run(port=5000, debug=True)