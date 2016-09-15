import os
import sys
import threading

from multiprocessing import Queue

def mkfifo(path):
    try:
        os.mkfifo(path)
    except OSError:
        # fifo already exists
        pass


class KeyboardCapture(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.run = self.fifo_loop
        #self.run = self.raw_input_loop
        
    def raw_input_loop(self):
        while(True):
            raw_stroke = raw_input()
            self.send_stroke(raw_stroke)

    def fifo_loop(self):
        path = "/tmp/plovin"
        mkfifo(path)
        while(True):
            fifo = open(path, 'r', 0)
            for raw_stroke in fifo:
                self.send_stroke(raw_stroke)
            fifo.close()


class PipeOut(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.q = Queue()
        self.setDaemon(True)

    def run(self):    
        path = '/tmp/plovout'
        mkfifo(path)
        while True:
            output = self.q.get()
            fifo = open(path, 'w')
            fifo.write(output)
            fifo.close()


class KeyboardEmulation(object):
    def __init__(self):
        pipeout = PipeOut()
        pipeout.start()
        self.q = pipeout.q
        self.output = self.use_fifo
        #self.output = self.use_print

    def use_print(self, string):
        print string 

    def use_fifo(self, string):
        self.q.put(string, block = False)

    def send_backspaces(self, b):
        self.output("\b" * b)
        #self.output(("\b"*b) + (" "*b) + ("\b"*b))

    def send_string(self, t):
        self.output("%s" % t)

    def send_key_combination(self, c):
        self.output("[%s]" % c)
