from flask import Flask, Response
from flask_cors import CORS
import time
import json
import random

app = Flask(__name__)
CORS(app) # Crucial: Allows React (port 5173) to connect to Flask (port 5000)

def generate_fake_packets():
    protocols = ["TCP", "UDP", "ICMP", "DNS", "HTTPS"]
    
    while True:
        time.sleep(1) # Simulate a new packet every second
        
        packet = {
            "id": ''.join(random.choices('abcdefghijklmnopqrstuvwxyz0123456789', k=9)),
            "timestamp": time.strftime("%H:%M:%S"),
            "source": f"192.168.1.{random.randint(1, 255)}",
            "destination": f"8.8.8.8",
            "protocol": random.choice(protocols),
            "size": random.randint(40, 1500),
            "info": "Live stream from Flask SSE backend!"
        }
        
        # SSE format absolutely requires "data: <string>\n\n"
        yield f"data: {json.dumps(packet)}\n\n"

@app.route('/stream')
def stream():
    # mimetype='text/event-stream' is what tells the browser to keep the connection open
    return Response(generate_fake_packets(), mimetype='text/event-stream')

if __name__ == '__main__':
    print("Starting NetScope Backend on http://localhost:5000...")
    app.run(port=5000, debug=True)