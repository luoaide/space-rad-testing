#!/usr/bin/env python3

# SOCKET PROGRAM Example from: https://realpython.com/python-sockets/
# PLOTTING AND SERIAL INPUT CODE FROM: https://thepoorengineer.com/en/arduino-python-plot/#arduino

import socket
import time
import collections
from threading import Thread
import pandas as pd
from datetime import datetime

class serverData:
    def __init__(self, serverIP, portNum, numDev):
        self.server_ip = serverIP
        self.port = portNum
        self.numDevices = numDev
        self.collecting = True
        self.rawData = []
        self.thd = None

        self.Board1 = open(r"../spaceraddata/board1-stats.log", "a")
        self.Board2 = open(r"../spaceraddata/board2-stats.log", "a")
        self.Board3 = open(r"../spaceraddata/board3-stats.log", "a")
        self.Board4 = open(r"../spaceraddata/board4-stats.log", "a")
        self.Board5 = open(r"../spaceraddata/board5-stats.log", "a")

        print("Trying to start server at: " + str(self.server_ip))
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.bind( (self.server_ip, self.port) ) #associates the socket object (above) with network address and port number
            self.sock.listen() #makes this socket a "listening" socket waiting for connections
            print("Server running at " + str(self.server_ip))
        except:
            print("Failed to start server at: " + str(self.server_ip))

    def collectData(self):    # retrieve data
        self.thd = Thread(target=self.interupt)
        self.thd.start()

        client_sock, client_ip = self.sock.accept()
        while self.collecting:
            data = client_sock.recv(1024)
            if not data:
                print('closing connection to ', ip)
                client.close()

            decoded_data = data.decode()
            print(decoded_data)

            #need DEVICE_ID TIME_STAMP CURRENT_READING
            decoded_data_split = decoded_data.split(" ")
            deviceID = decoded_data_split[0]
            timeStamp = decoded_data_split[1]
            serverTime = datetime.now().time()
            currentReading = float(decoded_data_split[2])

            self.rawData.append([deviceID, serverTime, timeStamp, currentReading])
            if deviceID == "Board_1_A0":
                self.Board1.write("{devID}, {serTime}, {devTime}, {current}\n".format(devID = deviceID, serTime = serverTime, devTime = timeStamp, current = currentReading))
            elif deviceID == "Board_2_A1":
                self.Board2.write("{devID}, {serTime}, {devTime}, {current}\n".format(devID = deviceID, serTime = serverTime, devTime = timeStamp, current = currentReading))
            elif deviceID == "Board_3_A2":
                self.Board3.write("{devID}, {serTime}, {devTime}, {current}\n".format(devID = deviceID, serTime = serverTime, devTime = timeStamp, current = currentReading))
            elif deviceID == "Board_4_A3":
                self.Board4.write("{devID}, {serTime}, {devTime}, {current}\n".format(devID = deviceID, serTime = serverTime, devTime = timeStamp, current = currentReading))
            elif deviceID == "Board_5_A4":
                self.Board5.write("{devID}, {serTime}, {devTime}, {current}\n".format(devID = deviceID, serTime = serverTime, devTime = timeStamp, current = currentReading))
        client_sock.close()
        self.closeout()

    def interupt(self):
        time.sleep(5)
        response = "run"
        while response != "stop":
            response = input("To stop data collection, type 'stop': ")
        print("stopping data collection...")
        self.collecting = False

    def closeout(self):
        self.thd.join()
        self.Board1.close()
        self.Board2.close()
        self.Board3.close()
        self.Board4.close()
        self.Board5.close()
        data = pd.DataFrame(self.rawData)
        data.to_csv('current-backup.csv')
        exit(0)

def main():
    #It will be a job of each Arduino to push data to this server.
    serverIP = "192.168.1.1"
    portNum = 65432
    numDev = 1
    #Incoming Data Format: DEVICE_ID TIME_STAMP CURRENT_READING

    s = serverData(serverIP, portNum,  numDev)      # initializes all required variables
    s.collectData()                                 # starts reading connected devices
    exit(0)

if __name__ == '__main__':
    main()
