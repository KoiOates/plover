"""
A stripped down Plover that just runs the engine and with only
text based input and output.
"""

import sys
import os
import plover.app as app
from plover.config import ASSETS_DIR, SPINNER_FILE, copy_default_dictionaries
from plover.oslayer.keyboardcontrol import KeyboardEmulation
from plover.machine.base import STATE_ERROR, STATE_INITIALIZING, STATE_RUNNING
from plover.machine.registry import machine_registry
from plover import log

from plover import __name__ as __software_name__
from plover import __version__
from plover import __copyright__
from plover import __long_description__
from plover import __url__
from plover import __credits__
from plover import __license__

class Output(object):
    def __init__(self, engine_command_callback, engine):
        self.engine_command_callback = engine_command_callback
        self.keyboard_control = KeyboardEmulation()
        print sys.argv 

    def send_backspaces(self, b):
        self.keyboard_control.send_backspaces(b)

    def send_string(self, t):
        self.keyboard_control.send_string(t)

    def send_key_combination(self, c):
        self.keyboard_control.send_key_combination(c)

    # TODO: test all the commands now
    def send_engine_command(self, c):
        result = self.engine_command_callback(c)
        if result and not self.engine.is_running:
            self.engine.machine.suppress_last_stroke(self.send_backspaces)

class PloverTUI(object):
    def __init__(self, config):
        self.config = config
        try:
            with open(config.target_file, 'rb') as f:
                self.config.load(f)
        except Exception:
            log.error('loading configuration failed, reseting to default', exc_info=True)
            self.config.clear()
        copy_default_dictionaries(self.config)

        self.steno_engine = app.StenoEngine()
        self.steno_engine.add_callback(lambda s: self._update_status(s))
        self.steno_engine.set_output(Output(self.consume_command, self.steno_engine))

        try:
            app.init_engine(self.steno_engine, self.config)
        except Exception:
            print 'engine initialization failed'
            log.error('engine initialization failed', exc_info=True)

    def _update_status(self, state):
        print "_update_status[%s]" % state
        if state:
            machine_name = machine_registry.resolve_alias(
                self.config.get_machine_type())
            print machine_name

    def consume_command(self, command):
        # The first commands can be used whether plover has output enabled or not.
        if command == self.COMMAND_RESUME:
            self.steno_engine.set_is_running(True)
            return True
        elif command == self.COMMAND_TOGGLE:
            self.steno_engine.set_is_running(not self.steno_engine.is_running)
            return True
        elif command == self.COMMAND_QUIT:
            self._quit()
            return True

    def MainLoop(self):
        while(True):
            pass

    def _quit(self, event=None):
        if self.steno_engine:
            self.steno_engine.destroy()
        print "quitting[%s]" % event
        sys.exit()

    def _reconnect(self):
        try:
            app.reset_machine(self.steno_engine, self.config)
        except Exception:
            log.error('machine reset failed', exc_info=True)
