import os
import threading
import time
import sys

from multiprocessing import Queue

from plover.key_combo import add_modifiers_aliases, parse_key_combo

def mkfifo(path):
    try:
        os.mkfifo(path)
    except OSError:
        # fifo already exists
        pass
# TODO verify that the keystroke sender still
# works as expected with these first modifications
# Then keep going through each key (and {#key}) and mapping it to a key code
# for HID, adding new ones from Hut1_12v2.pdf where necessary
# Also, veryify hold and tap behavior:
# --hold --left-alt --tab
##--hold --left-alt
##--hold --left-alt --tab
##--hold --left-alt
##--hold --left-alt --tab etc.

mods = [ "Control_L",
         "Control_R",
         "Shift_L",
         "Shift_R",
         "Alt_L",
         "Alt_R",
         "Meta_L",
         "Meta_R"]

#hid_gadget_test only recognized capitalized modifiers
#lowercase just sent the first letter of the description
complex_chars = {
    "control_l": "Control_l",
    "control_r": "Control_r",
    "shift_l":   "Shift_l",
    "shift_r":   "Shift_r",
    "alt_l":     "Alt_l",
    "alt_r":     "Alt_r",
    "meta_l":    "Meta_l",
    "meta_r":    "Meta_r",

    " ":"space",

    "A":"Shift_l a",
    "B":"Shift_l b",
    "C":"Shift_l c",
    "D":"Shift_l d",
    "E":"Shift_l e",
    "F":"Shift_l f",
    "G":"Shift_l g",
    "H":"Shift_l h",
    "I":"Shift_l i",
    "J":"Shift_l j",
    "K":"Shift_l k",
    "L":"Shift_l l",
    "M":"Shift_l m",
    "N":"Shift_l n",
    "O":"Shift_l o",
    "P":"Shift_l p",
    "Q":"Shift_l q",
    "R":"Shift_l r",
    "S":"Shift_l s",
    "T":"Shift_l t",
    "U":"Shift_l u",
    "V":"Shift_l v",
    "W":"Shift_l w",
    "X":"Shift_l x",
    "Y":"Shift_l y",
    "Z":"Shift_l z",
   
    "!":"Shift_l 1",
    "exclam":"Shift_l 1",
    "@":"Shift_l 2",
    "at":"Shift_l 2",
    "#":"Shift_l 3",
    "numbersign":"Shift_l 3",  #not sure if that's what I think it is, didn't work testing
    "$":"Shift_l 4",
    "dollar":"Shift_l 4",
    "%":"Shift_l 5",
    "percent":"Shift_l 5",
    "^":"Shift_l 6",
    # {#...} litteral?
    "&":"Shift_l 7",
    "ampersand":"Shift_l 7",
    "*":"Shift_l 8",
    "asterisk":"Shift_l 8",
    "(":"Shift_l 9",
    "parenleft":"Shift_l 9",
    ")":"Shift_l 0",
    "parenright":"Shift_l 0",
    "_":"Shift_l hyphen",
    "underscore":"Shift_l hyphen",
    "-":"hyphen",
    "+":"Shift_l equal",
    "plus":"Shift_l equal",
    "=":"equal",

    "{":"Shift_l bracketleft",
    "[":"bracketleft",
    "}":"Shift_l bracketright",
    "]":"bracketright",
    "|":"Shift_l backslash",
    "\\":"backslash",
    ":":"Shift_l semicolon",
    ";":"semicolon",
    '"':"Shift_l apostrophe",
    "'":"apostrophe",
    "<":"Shift_l comma",
    ",":"comma",
    ">":"Shift_l period",
    ".":"period",
    "?":"Shift_l slash",
    "/":"slash",  #TODO finish putting in {#...} names of remaining nonalphanumeric characters
   
    "`":"grave",
    "~":"Shift_l grave",
    "asciitilde":"Shift_l grave"
}

def expand_char(char):
    if char in complex_chars:
        return complex_chars[char]
    else:
        return char

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

            # The following ratsnest is just so I can see what I'm doing
            # while testing the program.
            if not output.startswith("Hold"):
                o = output.replace("Shift_l ", "")
                if len(o) == 2:
                    o = o.strip()
                    if output.startswith("Shift_l "):
                        if o.isalnum():
                            o = o.upper()
                        else:
                            o = "<s>" + o
                elif output.startswith("space"):
                    o = "_"
                else:
                    o = output
                print o,
                sys.stdout.flush()


class KeyboardEmulation(object):
    def __init__(self):
        pipeout = PipeOut()
        pipeout.start()
        self.q = pipeout.q

        self.output = self.use_fifo
        #self.output = self.use_print
        #self.output = self.use_tmux_cuz_life_is_short

    def use_print(self, string):
        print string,

    def use_fifo(self, string):
        # without delay, characters get dropped
        # when sending over usb
        time.sleep(0.01)
        self.q.put(string, block = False)

    def use_tmux_cuz_life_is_short(self, string):
        # This isn't cheating, this is just admitting
        # that I'm not very good at c and probably won't
        # have time to change that for a while.
        os.system("tmux send-keys -t 1 \"%s\";" % string)
        #os.system("tmux send-keys -t 1 Enter")

        #This is too slow, just saving it in case I need it.
        #Using tail -f from rc.local does the trick.

    def send_backspaces(self, b):
        for i in range(b):
            self.output("backspace\n")

    def send_string(self, string):
        for char in string:
            self.output(expand_char(char) + "\n")

    def send_key_combination(self, combo_string):
        key_events = [
            (keycode, True if pressed else False) for keycode, pressed
            in parse_key_combo(combo_string)
        ]
        # Could implement sticky keys right here, just have a chord for a sticky mod,
        # and a self.stickies list that prepopulates heldmods, and whose non-emptiness
        # keeps a blank line from being added at the end (instead would add a Hold
        # for join mod in self.stickies)
        heldkeys = []
        heldmods = []
        hid_commands = []
        for key, pressed in key_events:
            key = expand_char(key)
            if pressed:
                if key in mods:
                    heldmods.append(key)
                    #hid_commands[-1].append(key)
                else:
                    heldkeys.append(expand_char(key))
            else:
                if key in mods:
                    heldmods.remove(key)
                else:
                    heldkeys.remove(key)

            command = ' '.join(['Hold'] + heldmods + heldkeys)

            hid_commands.append(command)
            print("heldkeys-=%s=- heldmods-=%s=-" % (heldkeys, heldmods))
        for command in hid_commands:
            print "c:", command
            self.output(command + "\n")
       
