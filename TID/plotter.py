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



class serialPlot:
    def __init__(self, serverIP, portNum, plotLength, numDev):
        self.server_ip = serverIP
        self.port = portNum
        self.plotMaxLength = plotLength
        self.numDevices = numDev
        self.rawData = []
        self.data = collections.deque([0] * plotLength, maxlen=plotLength)

        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.csvData = []

        print("Trying to start server at: "" + str(server_ip))
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.bind( (serverIP, portNum) ) #associates the socket object (above) with network address and port number
            self.sock.listen() #makes this socket a "listening" socket waiting for connections
            print("Server running at " + str(server_ip))
        except:
            print("Failed to start server at: " + str(server_ip))

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target = self.backgroundThread)
            self.thread.start()

            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)


#I don't understand call to getSerialData
    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText):
        #FIX THIS METHOD!!!
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')

        value,  = struct.unpack('f', self.rawData)    # use 'h' for a 2 byte integer

        self.data.append(value)    # we get the latest data point and append it to our array
        lines.set_data(range(self.plotMaxLength), self.data)
        lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
        # self.csvData.append(self.data[-1])

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        client_sock, client_ip = self.sock.accept()
        while (self.isRun):
            data = client_sock.recv(1024)
            if not data:
                break
            client_sock.sendall("RECEIVED " + str(numBytes))
            #need DEVICE_ID TIME_STAMP CURRENT READING
            data = data.split(" ")
            deviceID = data[0]
            timeStamp = data[1]
            currentReading = data[2]

            self.rawData += [deviceID, timeStamp, currentReading]
            self.isReceiving = True

        #once isRun is False, then close the proxy socket.
        client_sock.close()

    def close(self):
        self.isRun = False
        #Will client_sock successfully close?
        self.thread.join()
        self.sock.close()
        print('Disconnected...')
        df = pd.DataFrame(self.csvData)
        df.to_csv('/home/Aiden.Roberts/Desktop/data.csv') #check file path


def main():
    #It will be a job of each Arduino to push data to this server.
    serverIP = "192.168.1.1"
    portNum = 65432
    numDev = 1
    #Incoming Data Format: (DEVICE_ID, TIME_STAMP, CURRENT READING)
    maxPlotLength = 100

    s = serverData(serverIP, portName, plotLength, numDev)      # initializes all required variables
    s.readData()                                                # starts background thread

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
    lineValueText = ax.text(0.50, 0.90, '', transform=axes.transAxes)
    anim = animation.FuncAnimation(plot, s.getSerialData, fargs=(lines, lineValueText, lineLabel, timeText), interval=pltInterval)    # fargs has to be a tuple

    pyplot.legend(loc="upper left")
    pyplot.show()

    s.close()


if __name__ == '__main__':
    main()
