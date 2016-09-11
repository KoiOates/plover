import threading
from plover.machine.base import StenotypeBase
from plover.oslayer.keyboardcontrol import KeyboardCapture


class NonKeyboard(StenotypeBase):
    KEYS_LAYOUT = KeyboardCapture.SUPPORTED_KEYS_LAYOUT
    ACTIONS = StenotypeBase.ACTIONS + ('arpeggiate',)

    def __init__(self, params):
        """Monitor the keyboard's events."""
        super(NonKeyboard, self).__init__()
        self.arpeggiate = params['arpeggiate']
        self._bindings = {}
        self._down_keys = set()
        self._released_keys = set()
        self._keyboard_capture = None
        self._last_stroke_key_down_count = 0
        self._update_bindings()

    def _update_bindings(self):
        self._bindings = dict(self.keymap.get_bindings())
        for key, mapping in list(self._bindings.items()):
            if 'no-op' == mapping:
                self._bindings[key] = None
            elif 'arpeggiate' == mapping:
                if self.arpeggiate:
                    self._bindings[key] = None
                    self._arpeggiate_key = key
                else:
                    # Don't suppress arpeggiate key if it's not used.
                    del self._bindings[key]

    def set_mappings(self, mappings):
        super(NonKeyboard, self).set_mappings(mappings)
        self._update_bindings()


    def start_capture(self):
        """Begin listening for output from the stenotype machine."""
        self._released_keys.clear()
        self._last_stroke_key_down_count = 0
        self._initializing()
        try:
            self._keyboard_capture = KeyboardCapture()
            self._keyboard_capture.key_down = self._key_down
            self._keyboard_capture.key_up = self._key_up
            self._keyboard_capture.send_stroke = self._send_stroke
            self._keyboard_capture.start()
        except Exception, e:
            print "in start_capture", e
            self._error()
            raise
        self._ready()

    def stop_capture(self):
        """Stop listening for output from the stenotype machine."""
        if self._keyboard_capture is not None:
            self._keyboard_capture.cancel()
            self._keyboard_capture = None
        self._stopped()

    def set_suppression(self, enabled):
        # Not a real keyboard, doesn't need suppression
        return
        suppressed_keys = self._bindings.keys() if enabled else ()
        self._keyboard_capture.suppress_keyboard(suppressed_keys)

    def suppress_last_stroke(self, send_backspaces):
        send_backspaces(self._last_stroke_key_down_count)

    def _key_down(self, key, pretranslated = False):
        """Called when a key is pressed."""
        if pretranslated:
            steno_key = key
        else:
            steno_key = self._bindings.get(key)
        print key
        assert key is not None
        if key in self._bindings:
            self._last_stroke_key_down_count += 1
        steno_key = self._bindings.get(key)
        print "key[%s] steno_key[%s]" % (key, steno_key)
        if steno_key is not None:
            self._down_keys.add(steno_key)

    def _send_stroke(self, raw_stroke):
        steno_keys = [self._bindings.get(key) for key in raw_stroke]
        steno_keys = [key for key in steno_keys if key is not None]
        if steno_keys:
            self._notify(steno_keys)

    def _key_up(self, key, pretranslated = False, force_send = False):
        """Called when a key is released."""
        if not force_send:
            assert key is not None
        if pretranslated:
            steno_key = key
        else:
            steno_key = self._bindings.get(key)
        
        if steno_key is not None:
            # Process the newly released key.
            self._released_keys.add(steno_key)
            # Remove invalid released keys.
            self._released_keys = self._released_keys.intersection(self._down_keys)

        # A stroke is complete if all pressed keys have been released.
        # If we are in arpeggiate mode then only send stroke when spacebar is pressed.
        send_strokes = bool(self._down_keys and
                            self._down_keys == self._released_keys)
        if force_send:
            send_strokes = True
        if self.arpeggiate:
            send_strokes &= key == self._arpeggiate_key
        if send_strokes:
            steno_keys = list(self._down_keys)
            if steno_keys:
                self._down_keys.clear()
                self._released_keys.clear()
                # MARKER
                print "Steno keys being sent[%s]" % steno_keys
                self._notify(steno_keys)
            self._last_stroke_key_down_count = 0
    
    @classmethod
    def get_option_info(cls):
        bool_converter = lambda s: s == 'True'
        return {
            'arpeggiate': (False, bool_converter),
        }
