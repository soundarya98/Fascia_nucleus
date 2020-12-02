#!/usr/bin/python           # This is server.py file

import json
import socket               # Import socket module
import pickle
import pandas as pd
import numpy as np

def nodeserv(transfdata):
  tfdata = transfdata.values.tolist()
  tfdatajson = json.dumps(tfdata)
  #print(tfdata)
  sn = socket.socket()
  hostn ='localhost'
  portn = 14564
  sn.bind((hostn, portn))
  print('nodeserv is running')
  sn.listen(5)
  print('after listening')
  while True:
    cn, addrn = sn.accept()
    print('Got connection from', addrn)
    #cn.send(bytes('Thank you for connecting','utf-8'))
    cn.send(bytes(tfdatajson, 'utf-8'))
    print(cn.recv(1024))
    cn.close()

s = socket.socket()         # Create a socket object
host = socket.gethostname() # Get local machine name
port = 12345                # Reserve a port for your service.
s.bind((host, port))        # Bind to the port
print('working now')
s.listen(5)                 # Now wait for client connection.
while True:
  c, addr = s.accept()     # Establish connection with client.
  print('Got connection from', addr)
  c.send(bytes('Thank you for connecting','utf-8'))
  data = b""
  while True:
    packet = c.recv(4096)
    if not packet: break
    data += packet
  #print(pickle.loads(data))
  nodeserv(pickle.loads(data))

c.close() # Close the connection
