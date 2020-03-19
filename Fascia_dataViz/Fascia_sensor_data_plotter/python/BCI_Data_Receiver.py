import socket
import sys
import time
import numpy as np
import threading
import struct

import time

class BCI_Data_Receiver(object):

    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.sock = None
        self.receiveBuff= bytes()
        self.dataBuff = []
        self.dataReadyCallback = None
        self.readingThread = None
        self.address = (self.ip, self.port)
        self.prev_time_stamp = 0

    def startConnection(self):
        """Start the socket connection"""
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        #For UDP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(self.address)
        print(self.address)
        # self.sock.connect(("192.168.1.10",35294))
        # self.sock.connect(("192.168.1.10",35295))
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
                      "IMU 1", "IMU 2", "IMU 3", "IMU 4", "IMU 5", "IMU 6", #"IMU 7", "IMU 8", "IMU 9",
                      "EDA  ", "TEMP ", "PPG  "]#, "BTR "]#"HRT  "]
        num_elements = 19
        num_bytes = 4*num_elements
        num_packets = 19
        while True:
            #Receive data from sensor
            data, addr = self.sock.recvfrom(num_bytes*num_packets)
            cur_time_stamp = time.time()
            print("data rate: "+str(int(num_packets/(cur_time_stamp-self.prev_time_stamp))) + " Hz")
            self.prev_time_stamp = time.time()
            # print(data, addr)
            #self.receiveBuff = self.receiveBuff + self.sock.recv(40)
            self.receiveBuff = self.receiveBuff + data
        
            if(len(self.receiveBuff) >= num_bytes*num_packets):
                data = self.receiveBuff[0:num_bytes*num_packets]
                self.receiveBuff = self.receiveBuff[num_bytes*num_packets:]
                for i in range(num_packets):
                    # For Junqing ADS1299
                    # unpacked_data = struct.unpack('qiiiiiiii', data[i*40: (i+1)*40])
                    # For FluidBCI
                    unpacked_data = struct.unpack('i'*10+'i'*6+'i'+'f'+'i', data[i*num_bytes: (i+1)*num_bytes])
                    # unpacked_data = struct.unpack('i'*num_elements, data[i*num_bytes: (i+1)*num_bytes])
                    #from manual For the 'f', 'd' and 'e' conversion codes, the packed representation uses the IEEE 754 binary32, binary64 or binary16 format (for 'f', 'd' or 'e' respectively), regardless of the floating-point format used by the platform.
                    # For Walaa: debug print
                    # for j in range(num_elements):
                    #     print(data_names[j] + ' ' + str(unpacked_data[j]))
                    # For Foc.us BCI
                    # unpacked_data = struct.unpack('iiffffffff', data[i*40: (i+1)*40])
                    self.dataReadyCallback(unpacked_data)