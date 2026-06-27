import subprocess
import json
import time
import random
from flask import Flask, Response

app = Flask(__name__)

@app.after_request
def add_cors_headers(response):
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    return response

def generate_packet_stream():
    # Spawns your compiled C sniffer binary as a root subprocess.
    # Ensure your C binary is compiled with: gcc sniffer.c -o sniffer
    process = subprocess.Popen(
        ['sudo', './sniffer'], 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE,
        text=True
    )

    try:
        while True:
            line = process.stdout.readline()
            if not line:
                break
            
            try:
                # Safely parse the strict JSON output directly from the C executable
                c_data = json.loads(line.strip())
                
                app_proto = c_data.get("app")
                layer4 = c_data.get("layer4")
                
                # Transform the data schema into the precise keys expected by React
                react_packet = {
                    "id": ''.join(random.choices('abcdefghijklmnopqrstuvwxyz0123456789', k=9)),
                    "timestamp": time.strftime("%H:%M:%S"),
                    "source": c_data.get("src_ip", "Unknown"),
                    "destination": c_data.get("dst_ip", "Unknown"),
                    "protocol": app_proto if app_proto not in ["TCP_OTHER", "TCP_CTRL", "UDP_OTHER"] else layer4,
                    "size": c_data.get("payload_bytes", 0),
                    "info": f"Src Port: {c_data.get('src_port')} -> Dst Port: {c_data.get('dst_port')}",
                    
                    # Week 4 Layer 7 Deep Inspection Fields
                    "http_method": c_data.get("http_method", ""),
                    "dns_query": c_data.get("dns_query", ""),
                    "smtp_cmd": c_data.get("smtp_cmd", "")
                }
                
                # Stream the formatted packet over HTTP SSE protocol
                yield f"data: {json.dumps(react_packet)}\n\n"
                
            except json.JSONDecodeError:
                # Catch any system string anomalies or sudo authentication text safely
                continue
    finally:
        process.terminate()

@app.route('/stream')
def stream():
    return Response(generate_packet_stream(), mimetype='text/event-stream')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)