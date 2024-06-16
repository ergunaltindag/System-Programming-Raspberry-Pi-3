# Remote Control System

This project aims to develop a remote control system based on Raspberry Pi 3. The system will run on a Raspberry Pi 3 and communicate with a PC (Client) over Ethernet. This document details the steps for compiling and running the project.

## Requirements

1. **Raspberry Pi 3**: The server software will run on this device.
2. **PC (Client)**: A PC running Ubuntu OS that will connect to the Raspberry Pi 3 via Ethernet.
3. **CMake**: CMake must be installed on both the Raspberry Pi 3 and the PC.
4. **GCC/G++**: A C++ compiler is required.

### Installing CMake

If CMake is not installed, you can install it using the following commands:

For Ubuntu:

sudo apt update
sudo apt install cmake

#### Configuration

In the ClientNode.cpp file on the client side, you need to specify the IP address of the Raspberry Pi. Replace the IP address in the file with the actual IP address of your Raspberry Pi 3. For example:
std::string raspberryPiIP = "192.168.1.10"; // Raspberry Pi IP adresi

##### Ports

Ports used in the project:

7001: Main connection port
7002: DigitalIO Node port
7003: Environment Sensor Node port
These ports are used on the Raspberry Pi 3. The PC will communicate with the Raspberry Pi 3 over Ethernet through these ports.

###### Compilation Instructions

Compiling on the Server (Raspberry Pi 3)
To compile the server software, navigate to the RCS/build directory and use CMake to start the build process:
cd RCS/build
cmake ..
make

Compiling on the Client (PC)
To compile the client software, navigate to the Client/build directory and use CMake to start the build process:
cd Client/build
cmake ..
make

####### Running the System
Server Side (Raspberry Pi 3)
On the Raspberry Pi 3, start the server software using the following commands:
./ServerNode
./EnvSensorNode
./DigitalIONode

Client Side (PC)
On the PC, start the client software using the following command:
./ClientNode

######## Notes
The system will only communicate using the specified ports. Using different ports may prevent the system from working correctly.
Ensure the IP address of the Raspberry Pi 3 is correctly configured, and the client software can access this IP address.
The project code is designed for the Ubuntu operating system. If you are using a different OS, you may need to make necessary adjustments.

