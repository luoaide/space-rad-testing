#!/usr/bin/env python3

# SOCKET PROGRAM Example from: https://realpython.com/python-sockets/
# PLOTTING AND SERIAL INPUT CODE FROM: https://thepoorengineer.com/en/arduino-python-plot/#arduino

import socket
from threading import Thread

import time
import collections
import matplotlib.pyplot as pyplot
import matplotlib.animation as animation
import struct
import pandas as pd


class serverData:
    def __init__(self, serverIP, portNum, plotLength, numDev):
        self.server_ip = serverIP
        self.port = portNum
        self.plotMaxLength = plotLength
        self.numDevices = numDev
        self.rawData = []
        self.data = collections.deque([0] * self.plotMaxLength, maxlen=self.plotMaxLength)

        self.thread = None

        self.isRun = True
        self.isReceiving = False
        self.plotTimer = 0
        self.previousTimer = 0
        self.csvData = []

        print("Trying to start server at: " + str(self.server_ip))
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.bind( (self.server_ip, self.port) ) #associates the socket object (above) with network address and port number
            self.sock.listen() #makes this socket a "listening" socket waiting for connections
            print("Server running at " + str(self.server_ip))
        except:
            print("Failed to start server at: " + str(self.server_ip))

    def readClientDataStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()

            # Block until we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)


    def getData(self, frame, lines, lineValueText, lineLabel, timeText):
        #getData is called by the plotting function in order to retreive the stored information.
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer

        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')

        #Incoming Data Stored in RawData as [DEVICE_ID, TIME_STAMP, CURRENT READING]
        #Newest data goes to the end of raw Data bcause list appending takes O(1)
        if len(self.rawData) > 0:
            devID = self.rawData[-1][0] #get the latest device ID
            timeStamp = self.rawData[-1][1] #the offset from epoch beginning when this current measurement was taken
            value = self.rawData[-1][2] # get the latest current measurment from self.rawData and put it on the queue (domain of plot)

            self.data.append(value)    # we get the latest data point and append it to our array (the domain of the plot)
            lines.set_data(range(0, self.plotMaxLength), self.data)
            lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
            self.csvData.append(self.rawData[-1])
        else:
            value = 0
            self.data.append(value)    # we get the latest data point and append it to our array (the domain of the plot)
            lines.set_data(range(self.plotMaxLength), self.data)
            lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
            self.csvData.append([0, 0, 0])

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data

        client_sock, client_ip = self.sock.accept()
        while self.isRun:
            data = client_sock.recv(1024)
            if not data:
                print('closing connection to ', ip)
                client.close()

            decoded_data = data.decode()
            print(decoded_data)
            #client_sock.sendall("RECEIVED".encode())

            #need DEVICE_ID TIME_STAMP CURRENT READING
            decoded_data_split = decoded_data.split(" ")
            deviceID = decoded_data_split[0]
            timeStamp = decoded_data_split[1]
            currentReading = float(decoded_data_split[2])

            self.rawData.append([deviceID, timeStamp, currentReading])
            self.isReceiving = True

        client_sock.close()

    def close(self):
        self.isRun = False
        time.sleep(1.0) #this should ensure that client_sock has time to close before we join the thread.
        self.thread.join()
        self.sock.close()
        print('Disconnected...')
        #df = pd.DataFrame(self.csvData)
        #df.to_csv('C:\Users\Aiden.Roberts\Desktop\spaceraddata\data.csv') #check file path


def main():
    #It will be a job of each Arduino to push data to this server.
    serverIP = "192.168.1.1"
    portNum = 65432
    numDev = 1
    #Incoming Data Format: DEVICE_ID TIME_STAMP CURRENT_READING
    maxPlotLength = 99

    s = serverData(serverIP, portNum, maxPlotLength, numDev)      # initializes all required variables
    s.readClientDataStart()                                     # starts background thread

    # plotting starts below
    pltInterval = 50    # Period at which the plot animation updates [ms]
    xmin = 0
    xmax = maxPlotLength
    ymin = 0            #current measurement min
    ymax = 10           #current measurement max
    plot = pyplot.figure()
    axes = pyplot.axes(xlim=(xmin, xmax), ylim=(float(ymin - (ymax - ymin) / 10), float(ymax + (ymax - ymin) / 10)))
    axes.set_title('Raspberry Pi Current Measurement')
    axes.set_xlabel("time")
    axes.set_ylabel("Current Measurment (V)")

    lineLabel = 'Current Value'
    timeText = axes.text(0.50, 0.95, '', transform=axes.transAxes)
    lines = axes.plot([], [], label=lineLabel)[0]
    lineValueText = axes.text(0.50, 0.90, '', transform=axes.transAxes)
    anim = animation.FuncAnimation(plot, s.getData, fargs=(lines, lineValueText, lineLabel, timeText), interval=pltInterval)    # fargs has to be a tuple

    pyplot.legend(loc="upper left")
    pyplot.show()

    s.close()


if __name__ == '__main__':
    main()
