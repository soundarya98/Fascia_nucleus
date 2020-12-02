#!/usr/bin/python           # This is server.py file

import json
import socket               # Import socket module
import pickle
import pandas as pd
import numpy as np

HEADERSIZE = 10

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

  while True:
    full_msg = b''
    new_msg = True
    while True:
      msg = c.recv(4096)
      print("receive working")
      if new_msg:
        print("new msg len:", msg[:HEADERSIZE])
        msglen = int(msg[:HEADERSIZE])
        new_msg = False
      print(f"full message length: {msglen}")
      full_msg += msg
      print(len(full_msg))
      if len(full_msg) -HEADERSIZE == msglen:
        print("full msg recvd")
        #print(full_msg[HEADERSIZE:])
        print(type(pickle.loads(full_msg[HEADERSIZE:])))
        new_msg = True
        full_msg = b""

  #print(pickle.loads(data))
  #nodeserv(pickle.loads(data))

c.close() # Close the connection
