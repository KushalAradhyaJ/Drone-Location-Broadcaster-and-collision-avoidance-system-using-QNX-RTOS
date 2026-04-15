# QNX-Based ADS-B Collision Avoidance System

A real-time distributed aviation safety simulation developed on the **QNX Neutrino RTOS**. This project simulates two drones (UAVs) broadcasting ADS-B data via UDP to a centralized Receiver and a Python-based Ground Station Radar.

## 🚀 System Architecture

The project is built on a **Producer-Consumer** model using a **Distributed Broadcast** architecture. It leverages the microkernel capabilities of QNX for fault isolation and real-time determinism.

- **Drones (Transmitters):** Multiple QNX processes simulating independent UAVs.
- **ADS-B Receiver:** A QNX process that performs "Data Fusion," calculating relative distances and predicting head-on collisions.
- **Ground Station (HMI):** A Python-based visualization tool that renders real-time drone positions on a radar display.

## 🛠 Tech Stack

| Component | Technology | Description |
| :--- | :--- | :--- |
| **Operating System** | QNX Neutrino RTOS | Real-time execution and microkernel stability. |
| **Networking** | UDP / IP Sockets | Low-latency, connectionless IPC via `io-sock`. |
| **Programming** | C (QNX) / Python (HMI) | Performance-critical logic and GUI visualization. |
| **Communication** | Binary Marshalling | C-structs mapped to Python using the `struct` library. |

## 📡 Networking & IPC
The system uses **UDP IPC over TCP/IP sockets**. Unlike TCP, UDP is chosen for its low overhead and "latest data" priority, which is essential for collision-critical aviation systems. 

- **Broadcasting:** Drones broadcast to `255.255.255.255:5555`, allowing multiple listeners.
- **Fault Isolation:** Each drone and the receiver run as independent **Processes**, ensuring that a failure in one does not compromise the others.

## 📊 Logic & Collision Avoidance
The Receiver tracks the state of all active Drone IDs. It implements a **Euclidean Distance** calculation between all pairs of drones. 
- **Safety Threshold:** 100 meters.
- **Alert System:** Triggers a `!!! WARNING: POTENTIAL COLLISION !!!` in the QNX console when the separation distance drops below the threshold.

## 💻 Setup & Execution
1. **QNX Side:**
   - Launch `io-sock` or `io-pkt` to enable networking.
   - Run `ADSB_Transmitter` and `ADSB_Transmitter2`.
   - Run `ADSB_Receiver`.
2. **Windows Side:**
   - Run `python radar_monitor.py`.

## 📜 Academic Concepts Demonstrated
- **Real-Time Determinism:** Guaranteed execution timing in QNX.
- **Inter-Process Communication (IPC):** Socket-based communication across a virtual network bridge.
- **Distributed Systems:** Multi-node simulation handling data concurrency.
- **HMI Design:** Human-Machine Interface for situational awareness.
