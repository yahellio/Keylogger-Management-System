# 🔐 Keylogger Management System

A full-stack system for remote data collection and management from keyboard clients. Developed as a course project.

## 🏗 System Architecture

1.  **Client (C):** A low-level program that captures keystrokes and saves them to a local file.
2.  **Client (Node.js):** A transport agent that periodically or on-demand (long-polling) sends collected data to the server.
3.  **Server (Node.js + Express):** Receives data from multiple clients, stores it in a file, and provides a REST API for the frontend.
4.  **Dashboard (React):** A web interface to view collected data and manage clients (activate/deactivate, request data).

## 🛠 Tech Stack

*   **Client:** C, Node.js
*   **Server:** Node.js, Express.js
*   **Frontend:** React
*   **Communication:** HTTP, Long Polling/REST API

## ✨ Key Features

*   **Data Collection:** Low-level input capture using C
*   **Data Transmission:** Periodic sending and long-polling mechanisms
*   **Web Dashboard:**
    *   List of connected devices
    *   View logs from each device
    *   Remote device management
    *   **Fully responsive design** with mobile support
*   **Security:**
    *   Focus on system integration and avoiding detection

## 📱 View

### Desktop Interface
<img width="866" height="431" alt="Desktop Interface" src="https://github.com/user-attachments/assets/3ba2cfc0-4bd9-4694-aa5f-ccae5ead1dbc" />

### Mobile Interface  
<img width="321" height="576" alt="Mobile Interface" src="https://github.com/user-attachments/assets/bdd0c778-cf3a-4841-9b53-b28b6438bf48" />



