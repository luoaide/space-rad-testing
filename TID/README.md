# TID Test Architecture

### current_measurment.ino
This script runs on an Arduino Controllino and reads in current measurements from the test devices. It then sends the output via ethernet to a python script on a listening Raspberry Pi Server.

### plotter.py
This script waits for input from the Controllino MAXI. It graphs the input (current measurement of test devices) in live-time and records the data.

### diagnostic.sh
This script runs on each test devices and sends data such as temperature, CPU usage, etc... over an ssh session (scp specifically).

### diagnostics.c
This script runs on each of the test devices and is called by diagnostic.sh. This diagnostic information includes a computationally complex task.



### Steps to setup each Pi:
1. configure NetPlan for static IP assignment
2. add ssh keys
3. change variables in /scripts files to match pi ID
4. install vcgendmd and add ubuntu user to video group (permissions)
    command: sudo apt install libraspberrypi-bin
    then: sudo usermod -aG video ubuntu
    log out and back in
    run with: vcgencmd measure_temp
    (or other commands)

    //https://elinux.org/RPI_vcgencmd_usage
    //https://2byt.es/post/vcgencmd/
    //https://chewett.co.uk/blog/258/vchi-initialization-failed-raspberry-pi-fixed/
