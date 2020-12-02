import socket
import sys
import time
import numpy as np
import threading
import struct

import time
import math

class BCI_Data_Receiver(object):

    def __init__(self, ip, port, data_plotting_widget, window):
        self.ip = ip
        self.port = port
        self.sock = None
        self.receiveBuff= bytes()
        self.dataBuff = []
        self.dataReadyCallback = None
        self.readingThread = None
        self.address = (self.ip, self.port)

        self.dataPlottingWidget = data_plotting_widget

        self.current_data_rate = 1

        self.prev_time_stamp = 0
        self.prev_A_ts = 0;

        self.num_data_to_halt = math.inf#5000 #TODO: set to 'math.inf' or comment-out lines 63-65 to run continuously
        self.num_data_so_far = 0;
        self.window = window

    def startConnection(self):
        """Start the socket connection"""
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        #For UDP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(self.address)
        print(self.address)
        # self.sock.connect(("192.168.1.10",35294))
        self.sock.settimeout(30.0)
        self.processStream()

    def asyncReceiveData(self, dataReadyCallback):
        """Receive the data from ADS1299 asynchronously."""
        if self.readingThread == None:
            self.dataReadyCallback = dataReadyCallback
            self.readingThread = threading.Thread(target = self.startConnection)
            self.readingThread.start()
        else:
            raise Exception("The reading thread is already running!")
    
    def processStream(self):
        data_names = ["PKT #", "VALID", "ADS 1", "ADS 2", "ADS 3","ADS 4", "ADS 5", "ADS 6", "ADS 7", "ADS 8",
                      "IMU 0", "IMU 1", "IMU 2", "IMU 3", "IMU 4", "IMU 5", #"IMU 7", "IMU 8", "IMU 9",
                      "EDA  ", "TEMP ", "PPG  ", "TIM"]#, "BTR "]#"HRT  "]
        num_elements = 17
        num_bytes = 4*num_elements
        num_packets = 20
        while True:
            # terminate process if reached self.num_data_to_halt
            if self.num_data_so_far >= self.num_data_to_halt:
                self.window.close()
                sys.exit()
            # Receive data from sensor
            data, addr = self.sock.recvfrom(num_bytes*num_packets)
            cur_time_stamp = time.time()
            # print("data rate: "+str(int(num_packets/(cur_time_stamp-self.prev_time_stamp))) + " Hz")
            self.current_data_rate = int(num_packets/(cur_time_stamp-self.prev_time_stamp))
            t = "P Data Rate: "+str(self.current_data_rate)+" Hz"
            self.dataPlottingWidget.PDR.setText(t)
            self.prev_time_stamp = time.time()
            # print(data, addr)
            self.num_data_so_far += num_packets
            #self.receiveBuff = self.receiveBuff + self.sock.recv(40)
            self.receiveBuff = self.receiveBuff + data
        
            if(len(self.receiveBuff) >= num_bytes*num_packets):
                data = self.receiveBuff[0:num_bytes*num_packets]
                self.receiveBuff = self.receiveBuff[num_bytes*num_packets:]
                for i in range(num_packets):
                    unpacked_data = struct.unpack('i'+'i'+'f'*8+'h'*6+'f'+'f'+'ii', data[i*num_bytes: (i+1)*num_bytes])
                    # unpacked_data = struct.unpack('i'*num_elements, data[i*num_bytes: (i+1)*num_bytes])
                    # from manual For the 'f', 'd' and 'e' conversion codes, the packed representation uses the IEEE 754 binary32, binary64 or binary16 format (for 'f', 'd' or 'e' respectively), regardless of the floating-point format used by the platform.

                    # print(unpacked_data[19], self.prev_A_ts)
                    # if unpacked_data[19] != self.prev_A_ts:
                    dr = 1000000/(unpacked_data[19] - self.prev_A_ts)
                    # print(dr)
                    t = "A Data Rate: " + str(int(dr)) + " Hz"
                    self.dataPlottingWidget.ADR.setText(t)
                    self.prev_A_ts = unpacked_data[19] # microseconds
                    # else:
                    #     print("same timestamp!")

                    # For Walaa: debug prints

                    # for j in range(num_elements):
                    #     print(data_names[j] + ' ' + str(unpacked_data[j]))

                    # if (unpacked_data[1] & (1<<16)) == 0:
                    #     # EDA is valid
                    #     print("EDA rate: " + str(int(1 / (cur_time_stamp - self.prev_EDA_time_stamp))) + " Hz")
                    #     self.prev_EDA_time_stamp = self.prev_time_stamp

                    self.dataReadyCallback(unpacked_data)