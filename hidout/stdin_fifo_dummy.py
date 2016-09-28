#!/usr/bin/python2.7
import os
import sys

print "starting raw_input loop to /tmp/plovin"
inp = ""
while(True):
    try:
        inp = raw_input()
    except EOFError, e:
        print "Caught EOFError."
    if inp == "stop":
        sys.exit(0)
    inp.replace("\\", "\\\\")
    inp.replace("'", "\\'")
    os.system('echo "%s" > /tmp/plovin' % inp)
