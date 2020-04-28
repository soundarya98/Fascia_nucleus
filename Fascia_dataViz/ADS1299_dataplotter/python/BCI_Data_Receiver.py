import socket
import sys
import time
import numpy as np
import threading
import struct
import math

class BCI_Data_Receiver(object):

    def __init__(self, ip, port, window):
        self.ip = ip
        self.port = port
        self.sock = None
        self.receiveBuff= bytes()
        self.dataBuff = []
        self.dataReadyCallback = None
        self.readingThread = None
        self.address = (self.ip, self.port)
        self.prev_time_stamp = 0

        self.num_data_to_halt = math.inf #TODO: set to 'math.inf' or comment-out lines 48-50 to run continuously
        self.num_data_so_far = 0;
        self.window = window

    def startConnection(self):
        """Start the socket connection"""
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        #For UDP
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(self.address)
        # self.sock.connect(("192.168.1.10",35294))
        # self.sock.connect(("192.168.1.10",35295))
        self.sock.settimeout(10.0) 
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
        while True:
            if self.num_data_so_far >= self.num_data_to_halt:
                self.window.close()
                sys.exit()

            #Receive data from sensor
            data, addr = self.sock.recvfrom(40*25)
            #print(data, addr)
            cur_time_stamp = time.time()
            # print("data rate: "+str(int(25/(cur_time_stamp-self.prev_time_stamp))) + " Hz")
            self.prev_time_stamp = time.time()
            self.num_data_so_far+=25

            #self.receiveBuff = self.receiveBuff + self.sock.recv(40)
            self.receiveBuff = self.receiveBuff + data
        
            if(len(self.receiveBuff) >= 40*25):
                data = self.receiveBuff[0:40*25]
                self.receiveBuff = self.receiveBuff[40*25:]
                for i in range(25):
                    # For Junqing ADS1299
                    # unpacked_data = struct.unpack('qiiiiiiii', data[i*40: (i+1)*40])
                    # For FluidBCI
                    unpacked_data = struct.unpack('iiiiiiiiii', data[i*40: (i+1)*40])
                    # For Foc.us BCI 
                    # unpacked_data = struct.unpack('iiffffffff', data[i*40: (i+1)*40])
                    self.dataReadyCallback(unpacked_data)