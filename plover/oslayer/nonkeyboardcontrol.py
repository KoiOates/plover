import sys
import threading

class KeyboardCapture(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        return

    def run(self):
        keylist = []
        count = 0
        while(True):
            raw_stroke = raw_input()
            print "Attempting to send [%s]" % raw_stroke
            self.send_stroke(raw_stroke)


class KeyboardEmulation(object):
    def __init__(self):
        return

    def send_backspaces(self, b):
        print "[bs*%d]" % b

    def send_string(self, t):
        print "sendstring[%s]" % t

    def send_key_combination(self, c):
        print "send_key_combination[%s]" % c
