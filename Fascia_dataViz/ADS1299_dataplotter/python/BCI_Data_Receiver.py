import socket
import sys
import time
import numpy as np
import threading
import struct


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
            #Receive data from sensor
            data, addr = self.sock.recvfrom(40*25)
            #print(data, addr)
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
            
