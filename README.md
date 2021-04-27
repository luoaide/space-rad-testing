# space-rad-testing

This repository is part of research project at the United States Military Academy sponsored by the PaNE Department. This project encompasses testing consumer off-the-shelf hardware (COTS Hardware) for durability in radiation environments. The goal of this research is to enable the use of cheaper, higher-performing CPUs such as the Raspberry Pi 3B+ and Raspberry Pi Compute Module in low-earth orbit satellites.

### Terminology 
TID: Total Ionizing Dose    
SEU: Singal Event Upset

# TID Test Structure

### Raspberry Pi image
The raspberry Pi needs to:
1. grab a static ip address
2. run diagnostic script on boot and send output over network

### current_measurment.ino
This script runs on an Arduino Uno and reads in current measurements from the test devices. It then sends the output serially to a python script on a listening host.

### plotter.py
This script waits for input from the ~8 arduino uno devices. It graphs the input (current measurement of test devices) in live-time and records the data.

### diagnostics.c
This script runs on each of the test devices and sends a standardized output to a waiting host. This diagnostic information includes computationally complex tasks and /log data such as temperature, cpu usage, etc...

### watchdog.py
This script accepts the diagnostic input from the test devices, logs, and presents it.

# SEU Test Structure
The SEU test is obsolete in 2021 iteration of this research. The files in the SEU folder were produced by Kyle King (D/EECS) and CDT Haley Duke in 2018. Those tests are non-operational and not used in the 2021 iteration.
