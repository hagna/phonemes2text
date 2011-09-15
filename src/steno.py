import time

class Keyer(object):
    """
    Use keyDown and keyUp as callbacks for triggering the decoder
    with the relevant chord of keys.


    @type  decoder: method
    @param decoder: called with the chord as a tuple like (0,1,2)

    """

    threshold = 100
    decoder = lambda a, b: None


    def __init__(self, decoder, threshold=None):
        if decoder is not None:
            self.decoder = decoder
        self.maxtime = 0
        self.buffer = []
        self.keyDownbuffer = []
        if threshold is not None:
            self.threshold = threshold


    def keyUp(self, event, timestamp):
        if timestamp - self.maxtime > self.threshold:
            self.buffer = []
        self.maxtime = timestamp
        self.buffer.append(event)
        if event in self.keyDownbuffer:
            self.keyDownbuffer.remove(event)
        if self.all_keys_up():
            self.decoder(tuple(self.buffer))
            self.buffer = []


    def keyDown(self, event, timestamp):
        self.keyDownbuffer.append(event)


    def all_keys_up(self):
        return self.keyDownbuffer == []
