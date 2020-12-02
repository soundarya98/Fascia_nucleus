#!/usr/bin/python           # This is client.py file

import socket               # Import socket module

s = socket.socket()         # Create a socket object
host = "ec2-15-207-84-227.ap-south-1.compute.amazonaws.com" # Get local machine name
#host = socket.gethostname()
port = 12345                # Reserve a port for your service.

s.connect((host, port))
print(s.recv(1024).decode('utf-8'))
s.close()                     # Close the socket when done