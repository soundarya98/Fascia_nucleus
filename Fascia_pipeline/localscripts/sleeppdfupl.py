import time
import threading
import socket
import numpy as np
import pandas as pd
import sys
from time import sleep
import pickle
import queue
import json

HEADERSIZE = 10
count = 0

q = queue.Queue()
s = socket.socket()

class RepeatedTimer(object):
  def __init__(self, interval, function, *args, **kwargs):
    self._timer = None
    self.interval = interval
    self.function = function
    self.args = args
    self.kwargs = kwargs
    self.is_running = False
    self.next_call = time.time()
    self.start()

  def _run(self):
    self.is_running = False
    self.start()
    self.function(*self.args, **self.kwargs)

  def start(self):
    if not self.is_running:
      self.next_call += self.interval
      self._timer = threading.Timer(self.next_call - time.time(), self._run)
      self._timer.start()
      self.is_running = True

  def stop(self):
    self._timer.cancel()
    self.is_running = False

def _load_npz_file(npz_file):
    """Load data and labels from a npz file."""
    with np.load(npz_file) as f:
        data = f["emg"]
        labels = f["labels"]
        #sampling_rate = f["fs"]
    return data, labels

def senddata(tbsent):
    #senddt = pickle.dumps(tbsent)
    #s.send(senddt)
    global count
    count = count + 100
    q.put(1)


def main():
    host = "ec2-35-154-162-208.ap-south-1.compute.amazonaws.com" # Get local machine name
    #host = socket.gethostname()
    port = 12345                # Reserve a port for your service.
    print('working now')
    s.connect((host, port))
    print(s.recv(1024).decode('utf-8'))

    data, labels = _load_npz_file('SC4001E0.npz')

    print(sys.getsizeof(data[0].tolist()))

    rt = RepeatedTimer(1, senddata, data[count])
    while True:
        q.get()
        msg = pickle.dumps(data[count-100:count])
        msg = bytes(f"{len(msg):<{HEADERSIZE}}", 'utf-8')+msg #left formatting the format string using :<
        s.send(msg)
        #s.send(np.savez(data[count]))
        q.task_done()
    #sleep(1000)
    #s.close()                     # Close the socket when done

if __name__ == "__main__":
    main ()
