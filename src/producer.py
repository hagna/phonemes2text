from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor, defer
from twisted.application.internet import MulticastServer

from twisted.internet.task import LoopingCall
from twisted.python import log
from twisted.python.usage import Options, UsageError

import pygame
import pygame.font, pygame.time, pygame.fastevent, pygame.draw
try:
    import json
except:
    import simplejson as json

MIDI=False
try:
    import pygame.midi
    from pygame.midi import MIDIIN

except Exception, e:
    print e
    MIDI=False
    MIDIIN=None

import os, sys, itertools
from steno import Keyer
from decoder import FlushingDecoder
from common import react

from pygame.locals import *


#wp51 colors
blue = (0,6,178)
gray = (170,170,170)
red = (167,2,0)
white = (255,255,255)


def update_status_msg(background, msg):
    msg = "%-15s" % msg
    if pygame.font:
        font = pygame.font.Font(None, 26)

        text = font.render(msg, 1, gray)
        textpos = text.get_rect(left=450,
                            bottom=300)
        overlap = pygame.Rect(textpos)
        overlap.w *= 2
        overlap.h *= 2
        pygame.draw.rect(background, blue, overlap)
        background.blit(text, textpos)
    else:
        print msg


def load_image(name, colorkey=None):
    fullname = os.path.join('data', name)
    try:
        image = pygame.image.load(fullname)
    except pygame.error, message:
        print 'Cannot load image:', name
        raise SystemExit, message
    image = image.convert()
    if colorkey is not None:
        if colorkey is -1:
            colorkey = image.get_at((0,0))
        image.set_colorkey(colorkey, RLEACCEL)
    return image, image.get_rect()


def helper_text():
#        for k, phonemeset in enumerate([consonants, vowels]):
#            for i, s in enumerate(phonemeset):
#                text = font.render(s, 1, gray)
#                textpos = text.get_rect(left=10+k*200,
#                                        bottom=30+i*20)
        image, imagerect = load_image("mcs_phonemes.bmp")
        background.blit(image, imagerect)
#         text = font.render('b', gray)
#         text = font.render('i', gray)
#         text = font.render('t', gray)
        



class Window(object):
    """
    Adapted from game https://code.launchpad.net/~game-hackers/game/trunk

    A top-level PyGame-based window. This acts as a container for
    other view objects.

    @ivar clock: Something providing
        L{twisted.internet.interfaces.IReactorTime}.
    @ivar screen: The L{pygame.Surface} which will be drawn to.
    @ivar controller: The current controller.

    @ivar display: Something like L{pygame.display}.
    @ivar event: Something like L{pygame.event}.

    """
    screen = None

    def __init__(self,
                 clock=reactor,
                 display=pygame.display,
                 event=pygame.fastevent):
        self.clock = clock
        self.display = display
        self.controller = None
        self.event = event



    def paint(self):
        """
        Call C{paint} on all views which have been directly added to
        this Window.
        """
        self.display.flip()


    def handleInput(self):
        """
        Retrieve outstanding pygame input events and dispatch them.
        """
        global piano
        event = self.event.poll()
        if MIDI:
            if piano.poll():
                midi_events = piano.read(10)
                midi_evs = pygame.midi.midis2events(midi_events, piano.device_id) 

                for m_e in midi_evs:
                    pygame.fastevent.post( m_e )


        if event:
            self._handleEvent(event)


    def _handleEvent(self, event):
        """
        Handle a single pygame input event.
        """
        if event.type == pygame.locals.QUIT or \
                event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            self.stop()
        elif self.controller is not None:
            self.controller.dispatch_event(event)
#             elif event.type == pygame.MOUSEMOTION:
#                 if pygame.event.get_grab():
#                     self.controller.mouseMotion(
#                         event.pos, event.rel, event.buttons)
#             elif event.type == pygame.MOUSEBUTTONUP:
#                 pygame.event.set_grab(not pygame.event.get_grab())
#                 pygame.mouse.set_visible(not pygame.mouse.set_visible(True))


    def submitTo(self, controller):
        """
        Specify the given controller as the one to receive further
        events.
        """
        self.controller = controller
        self.controller.window = self


    def go(self):
        """
        Show this window.

        @return: A Deferred that fires when this window is closed by the user.
        """
        os.environ['SDL_VIDEO_CENTERED'] = '1'
        pygame.init()
        pygame.fastevent.init()

        self.display.init()
        self.screen = self.display.set_mode(
            (800,600),
            pygame.locals.DOUBLEBUF)

        image, imagerect = load_image("mcs_phonemes.bmp")
        self.screen.blit(image, imagerect)
        self._renderCall = LoopingCall(self.paint)
        self._renderCall.start(1 / 60, now=False)
        self._inputCall = LoopingCall(self.handleInput)
        finishedDeferred = self._inputCall.start(0.04, now=False)
        finishedDeferred.addCallback(lambda ign: self._renderCall.stop())
        finishedDeferred.addCallback(lambda ign: self.display.quit())

        return finishedDeferred


    def stop(self):
        """
        Stop updating this window and handling events for it.
        """
        self._inputCall.stop()


class Keymap:
    RHAND = [0,1,2,3,4,5]
    LHAND = [6,7,8,9,10,11]


    keyboardkeymap = {0: 3, 263: 2, 265: 4,
                      270: 5, 48: 9, 305: 6,
                      274: 0, 275: 1, 117: 11,
                      313: 7, 57: 10, 91: 8}


    pianokeymap =  {
                    72:5,
                    71:4,
                    69:3,
                    67:2,
                    65:1,
                    64:0,
                    62:6,
                    60:7,
                    59:8,
                    57:9,
                    55:10,
                    53:11,
                    }

    _learn_keymap = itertools.cycle([('Right hand 5', 5),
                                 ('Right hand 4', 4),
                                 ('Right hand 3', 3),
                                 ('Right hand 2', 2),
                                 ('Right hand 1', 1),
                                 ('Right hand 0', 0),
                                 ('Left hand 5', 11),
                                 ('Left hand 4', 10),
                                 ('Left hand 3', 9),
                                 ('Left hand 2', 8),
                                 ('Left hand 1', 7),
                                 ('Left hand 0', 6),
                                 None,
                         ]).next

    def __init__(self):
        self.load_keymap()

    def load_keymap(self):
        try:
            fh = open('keymap.json', 'r')
            newone = json.load(fh)
            self.keyboardkeymap.clear()
            for i in newone:
                self.keyboardkeymap[int(i)] = newone[i]
        except Exception, e:
            print e


    def dump_keymap(self):
        try:
            fh = open('keymap.json', 'w')
            json.dump(self.keyboardkeymap, fh)
        except Exception, e:
            print e


    def updateKeymap(self):
        self.keyboardkeymap.clear()
        rhand = self.RHAND[:]
        lhand = self.LHAND[:]
        rhand.reverse()
        lhand.reverse()
        for i, key in enumerate(rhand + lhand):
            self.keyboardkeymap[self.newmap[i]] = key
        self.newmap = []


    def lookup(self, k, validkeys=None):
        if validkeys is None:
            validkeys = self.keyboardkeymap
        res = validkeys.get(k, None)
        hand = 0
        if res in self.LHAND:
            hand = 1
            res = res - self.LHAND[0]
        return res, hand


    def lookup_piano(self, k):
        res = self.lookup(k, self.pianokeymap)
        return res


    newmap = []

    def keyDown(self, k):
        v = self._learn_keymap()
        self.newmap.append(k)

        if v == None:
            self.window.submitTo(self.parentController)
            update_status_msg(self.window.screen, '')
            self.updateKeymap()
            self.dump_keymap()
        else:
            msg, val = v
            update_status_msg(self.window.screen, msg)


    def keyUp(self, k):
        pass



class Controller:

    def __init__(self, decoder=None):
        if decoder is None:
            f = FlushingDecoder(0.500)
            def updateF():
                f.update(reactor.seconds())
            s = LoopingCall(updateF)
            s.start(0.04, now=False)
        else:
            f = decoder
        self.keymap = Keymap()
        self.lookup = self.keymap.lookup
        self.lkeyer = Keyer(f.decoder, 1500)
        self.rkeyer = Keyer(f.decoder, 1500)


    def dispatch_event(self, event):
        if event.type == pygame.KEYDOWN:
            self.keyDown(event.key)
        elif event.type == pygame.KEYUP:
            self.keyUp(event.key)


    def keyDown(self, k):
        if k == pygame.K_F3:
            self.window.submitTo(self.keymap)
            self.keymap.parentController = self
            update_status_msg(self.window.screen, self.keymap._learn_keymap()[0])
        t = reactor.seconds()
        z, hand = self.lookup(k)
        if z is None:
            return
        if hand == 0:
            self.rkeyer.keyDown(z, t)
        else:
            self.lkeyer.keyDown(z, t)


    def keyUp(self, k):
        t = reactor.seconds()
        z, hand = self.lookup(k)
        if z is None: return
        if hand == 0:
            self.rkeyer.keyUp(z, t)
        else:
            self.lkeyer.keyUp(z, t)



class MidiController(Controller):
    def __init__(self, *a, **kw):
        Controller.__init__(self, *a, **kw)
        self.lookup = self.keymap.lookup_piano
   

    def dispatch_event(self, event):
        if event.type == MIDIIN:
            note, status = event.data1, event.status
            if status == 144: # keydown
                self.keyDown(note)
            if status == 128: # keyup
                self.keyUp(note)





class MulticastClientUDP(DatagramProtocol):

    def datagramReceived(self, datagram, address):
        print "Received:" + repr(datagram)
        self.address = address
        print address



class Helloer(DatagramProtocol):

    def __init__(self, host, port):
        self.host = host
        self.port = port


    def startProtocol(self):
        host = self.host
        port = self.port
        self.transport.connect(host, port)
        print "now we can only send to host %s port %d" % (host, port)
        self.transport.write("hello") # no need for address

    def sendDone(self):
        self.transport.write('done')

    def datagramReceived(self, data, (host, port)):
        print "received %r from %s:%d" % (data, host, port)

    def decoder(self, b):
        z = ' '.join([str(k) for k in list(b)])
        self.transport.write(z)

    # Possibly invoked if there is no server listening on the
    # address to which we are sending.
    def connectionRefused(self):
        print "No one listening"



class RunOptions(Options):
    def getSynopsis(self):
        return """fqdn port"""

    optFlags = [('multi', 'm', 'Enable multicast')]


    def parseArgs(self, host, port):
        self.host = host
        self.port = int(port)



class StartOptions(Options):
    def getSynopsis(self):
        return """Usage: %s scan|fetch|put""" % __file__


    optParameters = [('midi', 'm', None, 'Midi channel to use. python -m pygame.examples.midi --list to see possibilities')]

    subCommands = [
        ('udp', None, RunOptions, 'Send phonemes over the network.'),
        ]


def midi_or_keyboard(opt):
    global MIDI, piano
    midi = opt.get('midi')
    if midi is None:
        return Controller
    device_id = int(midi)
    pygame.midi.init()
    if device_id is None:
        input_id = pygame.midi.get_default_input_id()
    else:
        input_id = device_id

    print ("using input_id :%s:" % input_id)
    piano = pygame.midi.Input( input_id )


    MIDI=True
    return MidiController

def parseCmdLine(argv):
    opt = StartOptions()
    try:
        opt.parseOptions(argv[1:])
    except UsageError, e:
         raise SystemExit(str(e))
    return opt


def main(reactor, argv):
    opt = parseCmdLine(argv)
    command = opt.subCommand
    controller = midi_or_keyboard(opt)
    if command == 'udp':
        so = opt.subOptions

        if so['multi']:
            mcastUDP = MulticastClientUDP()
            a = reactor.listenUDP(0, mcastUDP)
            class Decoder:
                def __init__(self, a):
                    self.a = a

                def decoder(self, b):
                    self.a.write(str(b), ('224.0.0.1', 8005))

            w = Window(reactor)
            w.submitTo(controller(Decoder(a)))
            d = w.go()
            return d
        else: 
            h = Helloer(so.host, so.port)            
            w = Window(reactor)
            w.submitTo(controller(h))
            d = w.go()
            d.addCallback(lambda a: h.sendDone())
            reactor.listenUDP(0, h)
            return d

    else:
        w = Window(reactor)
        w.submitTo(controller())
        d = w.go()
        return d

if __name__ == '__main__':
    react(reactor, main, sys.argv)



