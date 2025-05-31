# 🚦 AI-Powered Intelligent Traffic Management System

A Reinforcement Learning and Graph Neural Network-based traffic optimization system integrated with IoT modules (ESP8266) and SUMO simulation to dynamically manage traffic flow, prioritize emergency vehicles, and reduce congestion in urban environments.

---

## 📌 Table of Contents

- [Overview]
- [System Architecture]
- [Features]
- [Tech Stack]
- [Setup Instructions]
- [Hardware Setup]
- [Simulation with SUMO]
- [Flask Server & Real-time Logs]
- [Future Enhancements]


---

## ✅ Overview

This project proposes an intelligent traffic control system that combines:

- **Reinforcement Learning (Q-Learning)** for adaptive signal control.  
- **Graph Neural Networks (GNN)** to predict and optimize traffic at intersections.  
- **ESP8266 modules** for real-time communication between vehicles and infrastructure.  
- **SUMO (Simulation of Urban Mobility)** for modeling traffic flow and testing scenarios.  
- **Emergency vehicle prioritization** through MAC-based detection and dynamic signal rerouting.

The aim is to simulate and deploy a scalable, low-cost system for smart cities that adapts to dynamic traffic conditions.

---

## 🧠 System Architecture

<img width="392" alt="image" src="https://github.com/user-attachments/assets/ae959a19-c2b1-42cb-8d97-77290bcaf79b" />


---

## ✨ Features

- Real-time emergency vehicle detection using MAC identification.
- Dynamic traffic signal control using Q-Learning.
- Graph Neural Network-based congestion prediction.
- SUMO integration for simulation and validation.
- Flask backend for communication, control logic, and logging.
- ESP8266 modules for V2V and V2I communication.
- Traffic log visualization for debugging and analysis.

---

## 🧰 Tech Stack

| Technology              | Purpose                                 |
|------------------------|-----------------------------------------|
| **Python**             | Backend logic and simulation interface  |
| **Flask**              | Web server for real-time traffic control |
| **SUMO**               | Urban traffic simulation                 |
| **ESP8266**            | IoT module for vehicle-to-infrastructure |
| **HTML/CSS/JS**        | Real-time monitoring dashboard           |
| **Q-Learning**         | Adaptive traffic signal control          |
| **Graph Neural Networks** | Traffic prediction and optimization  |

---

## ⚙️ Setup Instructions

### 1. Clone the Repository

git clone https://github.com/ak909412/Smart-Traffic-Management.git
cd ai-traffic-management

### 2. Install Dependencies
pip install -r requirements.txt

### 3. Set Up SUMO
Install SUMO

Ensure sumo, sumo-gui, and traci are accessible in your PATH.

### 4. Configure Flask Server

cd backend
python app.py

## Hardware Setup
### Required Components:
1x ESP8266 NodeMCU for each traffic light

1x ESP8266 for emergency vehicle (hotspot mode)

Relay module to control LEDs (representing traffic signals)

Optional: RFID module, GPS for real-world deployment

### ESP8266 Logic:
Traffic Light ESP connects to vehicles and forwards MACs.

Emergency Vehicle ESP broadcasts hotspot with priority MAC.

Signals are controlled via GPIO based on server response.

## Simulation with SUMO
### File Structure:
/sumo/: SUMO configuration files

/models/: GNN training scripts and logs

/simulation.py: Interfaces SUMO with RL logic

/data/: Generated traffic datasets in XML format

### Run Simulation:

python simulation.py
You can switch between scenarios (low/high traffic, with/without emergency vehicles) using flags or modifying the net.xml and rou.xml.

## Flask Server & Real-time Logs
### Live Logging:

http://localhost:5000/logs

### API Endpoints:
POST /update_traffic: Receives MAC data

GET /logs: Streams recent traffic decisions

GET /status: Traffic light status per intersection

## Future Enhancements
✅ Federated Reinforcement Learning (FRL)

✅ 5G-enabled V2X communication

✅ Blockchain for traffic data integrity

✅ Digital Twin for live simulation

✅ Autonomous vehicle route coordination
