from flask import Flask, request, jsonify, Response
import time

app = Flask(__name__)

# Traffic Data Storage
traffic_data = {
    "TrafficLight_1": [],
    "TrafficLight_2": [],
    "TrafficLight_3": [],
    "TrafficLight_4": []
}

PRIORITY_MAC = "dc:4f:22:7b:23:f6"

vehicle_database = {
    "48:3f:da:57:90:51": {"owner": "John Doe", "type": "Car", "color": "Red", "number": "TN 10 AB 1234"},
    "c8:2b:96:2e:2e:29": {"owner": "Alice Smith", "type": "Bike", "color": "Black", "number": "KA 01 XY 5678"},
    "d8:bf:c0:11:71:92": {"owner": "Bob Johnson", "type": "Truck", "color": "Blue", "number": "MH 12 CD 9012"},
    "48:3f:da:63:6a:bf": {"owner": "Sawan", "type": "Car", "color": "Blue", "number": "DL 10 CA 9112"},
    "48:3f:da:64:50:fd": {"owner": "Anu", "type": "Car", "color": "Black", "number": "HR 26 EA 1232"},
    "8c:aa:b5:53:15:24": {"owner": "Ankit", "type": "Bike", "color": "Red", "number": "BR 26 PD 1201"},
    PRIORITY_MAC: {"owner": "Ambulance Service", "type": "Ambulance", "color": "White", "number": "AMB 786"}
}

log_messages = []  # Store logs for streaming

def decide_signal():
    return max(traffic_data, key=lambda key: len(traffic_data[key]))

@app.route('/update', methods=['POST'])
def update_traffic():
    global log_messages
    data = request.get_json()
    light_id = data['light_id']
    mac_addresses = data['macs']
    sender_ip = request.remote_addr

    mac_list = [mac.strip() for mac in mac_addresses.split(',') if mac.strip()]
    traffic_data[light_id] = mac_list  

    log_message = f"📡 Received from {sender_ip} → {light_id}: {mac_list}"
    log_messages.append(log_message)
    print(log_message)

    for mac in mac_list:
        vehicle_info = vehicle_database.get(mac, None)
        vehicle_msg = f"🆔 MAC: {mac} → {vehicle_info if vehicle_info else 'Unknown Vehicle'}"
        log_messages.append(vehicle_msg)
        print(vehicle_msg)

    for light, macs in traffic_data.items():
        if PRIORITY_MAC in macs:
            signal_status = "green" if light_id == light else "red"
            priority_msg = f"🚨 **Priority Vehicle Detected!** {light} gets Green Light."
            log_messages.append(priority_msg)
            print(priority_msg)
            return jsonify(signal_status)

    green_light = decide_signal()
    signal_status = "green" if light_id == green_light else "red"
    
    return jsonify(signal_status)

@app.route('/logs')
def stream_logs():
    def event_stream():
        last_sent_index = 0
        while True:
            if last_sent_index < len(log_messages):
                yield f"data: {log_messages[last_sent_index]}\n\n"
                last_sent_index += 1
            time.sleep(1)  

    return Response(event_stream(), mimetype="text/event-stream")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
