from twisted.internet import reactor, defer
from twisted.internet.task import LoopingCall
from twisted.python import log
from twisted.python.usage import Options, UsageError

import pygame
import pygame.font, pygame.time, pygame.fastevent, pygame.draw
try:
    import json
except:
    import simplejson as json


MIDI=True
try:
    pygame.midi
    from pygame.midi import MIDIIN

except Exception, e:
    print e
    MIDI=False
    MIDIIN=None

import os, sys, itertools
from steno import Keyer

from pygame.locals import *


#wp51 colors
blue = (0,6,178)
gray = (170,170,170)
red = (167,2,0)
white = (255,255,255)

def react(reactor, main, argv, **kw):
    """
    Call C{main} and run the reactor until the L{Deferred} it returns fires.

    @param reactor: An unstarted L{IReactorCore} provider which will be run and
        later stopped.

    @param main: A callable which returns a L{Deferred}.  It should take as
        many arguments as there are elements in the list C{argv}.

    @param argv: A list of arguments to pass to C{main}.

    @return: C{None}
    """
    stopping = []
    reactor.addSystemEventTrigger('before', 'shutdown', stopping.append, True)
    finished = main(reactor, argv, **kw)
    finished.addErrback(log.err, "main function encountered error")
    exit = []
    def cbFinish(ignored):
        exit.append(ignored)
        if not stopping:
            reactor.callWhenRunning(reactor.stop)
    finished.addCallback(cbFinish)
    reactor.run()
    raise SystemExit(exit[0])


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
        event = self.event.poll()
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
            if event.type == pygame.KEYDOWN:
                self.controller.keyDown(event.key)
            elif event.type == pygame.KEYUP:
                self.controller.keyUp(event.key)
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
                    70:4,
                    68:3,
                    66:2,
                    64:1,
                    62:0,
                    60:1,
                    58:2,
                    56:3,
                    54:4,
                    52:5,
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
        return self.lookup(k, Controller.pianokeymap)


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

    def __init__(self):
        f = FlushingDecoder(0.500)
        self.keymap = Keymap()
        self.lookup = self.keymap.lookup
        def updateF():
            f.update(reactor.seconds())
        s = LoopingCall(updateF)
        s.start(0.04, now=False)
        self.lkeyer = Keyer(f.decoder, 1500)
        self.rkeyer = Keyer(f.decoder, 1500)


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




if MIDI:
    device_id = 5
    pygame.midi.init()
    if device_id is None:
        input_id = pygame.midi.get_default_input_id()
    else:
        input_id = device_id

    print ("using input_id :%s:" % input_id)
    piano = pygame.midi.Input( input_id )




statusmsg = None


try:
    from espeak import espeak 
    synth = espeak.synth
    espeak.set_parameter(espeak.Parameter.Rate, 100)
    espeak.set_parameter(espeak.Parameter.Volume, 100)
    #espeak.set_voice('mb-us1')
    espeak.synth("hello")
    #print espeak.get_parameter("rate")
except Exception, e:
    print e
    print "no espeak"
    synth = None

import itertools
newvoice = itertools.cycle(['en-scottish', 'english', 'lancashire', 'english_rp', 'english_wmids', 'english-us', 'en-westindies']).next


espeak_consonants = {(4,): 'n', # consonants
                   (3,): 't',
                   (1,): 'r',
                   (2,): 's',
                   (5,): 'd',
                   (1,4): 'l',
                   (2,3): 'D',
                   (3,4): 'z',
                   (1,2): 'm',
                   (2,3,4): 'k',
                   (1,3): 'v',
                   (1,2,3,4): 'w',
                   (1,2,3): 'p',
                   (1,5): 'f',
                   (4,5): 'b',
                   (2,4): 'h',
                   (2,3,4,5): 'N',
                   (1,3,4): 'S',
                   (3,4,5): 'g',
                   (1,2,3,4,5): 'j',
                   (2,5): 'tS',
                   (1,4,5): 'dZ',
                   (1,2,4): 'T',
                   (1,3,4,5): 'Z3'}

espeak_vowels = {(0,): '@', # vowels
                   (0,4): 'I2',
                   (0,2): '0',
                   (0,1): 'I',
                   (0,3): 'a',
                   (0,2,3,4): 'E',
                   (0,2,3): 'i:',
                   (0,5): 'eI',
                   (0,3,4): 'V',
                   (0,2,3,4,5): 'U:',
                   (0,4,5): 'aI',
                   (0,3,4,5): 'U',
                   (0,2,5): '3:',
                   (0,2,3,5): 'aU',
                   (0,3,5): 'ju:',
                   (0,2,4,5): 'OI'}

espeak_phonemes = {}
espeak_phonemes.update(espeak_vowels)
espeak_phonemes.update(espeak_consonants)



class FlushingDecoder:

    def __init__(self, timeout):
        self.timeout = timeout
        self.buffer = []
        self.lastdecode = 0

    def flush(self):
        if self.buffer:
            s = ''.join(self.buffer)
            synth("[[" + s + "]]", phonemes=True)
            self.buffer = []

    def update(self, ticks):
        self.ticks = ticks
        n = self.ticks - self.lastdecode
        if self.ticks - self.lastdecode > self.timeout:
            self.flush()

    def decoder(self, b):
        self.lastdecode = self.ticks
        l = list(b)
        l.sort()
        phone = espeak_phonemes.get(tuple(l), None)
        if synth is None or phone is None:
            print phone
        else:
            self.buffer.append(phone)
            print self.buffer






# f = FlushingDecoder(500)
# lkeyer = Keyer(f.decoder, 1500)
# rkeyer = Keyer(f.decoder, 1500)
# learning = False
# listeners = []
# load_keymap()
# try:
#     screen = pygame.display.set_mode((800, 600))
#     pygame.display.set_caption('simplesteno')
#     pygame.mouse.set_visible(0)
#     background = pygame.Surface(screen.get_size())
#     background = background.convert()
#     background.fill(blue)
#     helper_text()
#     screen.blit(background, (0, 0))
#     pygame.display.flip()
#     clock = pygame.time.Clock()
#     while True:
#         screen.blit(background, (0, 0))
#         pygame.display.flip()

#         clock.tick(60)
#         t = pygame.time.get_ticks()
#         f.update(t)
#         event = event_poll()
#         if MIDI:
#            if piano.poll():
#                 midi_events = piano.read(10)
#                 # convert them into pygame events.
#                 midi_evs = pygame.midi.midis2events(midi_events, piano.device_id)

#                 for m_e in midi_evs:
#                     event_post( m_e )


#         if event.type == pygame.QUIT:
#             break
#         if event.type in (KEYDOWN, KEYUP):
#             key = event.key
#             z = lookup(key)
#             if event.type == KEYDOWN:
#                 if key == pygame.K_ESCAPE:
#                     break
#                 if learning:
#                     newkeymap.append(key)
#                     if len(newkeymap) == 12:
#                         learning = False
#                         updatekeymap(newkeymap, keyboardkeymap)
#                         update_status_msg('')
#                         dump_keymap()
#                         continue
#                     update_status_msg(_learn_keymap()[0])
#                 if key == 284:
#                     newkeymap = []
#                     learning = True
#                     update_status_msg(_learn_keymap()[0])
#                     continue
#                 for m in listeners:
#                     m(event)
#                 if z in RHAND:
#                     rkeyer.keyDown(z, t)
#                 if z in LHAND:
#                     lkeyer.keyDown(z-LHAND[0], t)
#             if event.type == KEYUP:
#                 if z in RHAND:
#                     rkeyer.keyUp(z, t)
#                 if z in LHAND:
#                     lkeyer.keyUp(z-LHAND[0], t)
#         if event.type == MIDIIN:
#             note, vel = event.data1, event.data2
#             z = lookup_piano(note)
#             if vel > 0: # keyDown

#                 if z is not None:
#                     keyer.keyDown(z, t)
#             if vel == 0: # keyUp
#                 if z is not None:
#                     keyer.keyUp(z, t)
# finally:
#     if MIDI:
#         if piano:
#             del piano
#         pygame.midi.quit()
#     pygame.quit()  # Keep this IDLE friendly 


class RunOptions(Options):
    def getSynopsis(self):
        return """port fqdn"""

#     def parseArgs(self, ip, fqdn):
#         self.ip = ip
#         self.fqdn = fqdn


class StartOptions(Options):
    def getSynopsis(self):
        return """Usage: %s scan|fetch|put""" % __file__

    subCommands = [
        ('net', None, RunOptions, 'Send phonemes over the network.'),
        ]

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
    if command == 'net':
        print "Not implemented yet."
        return defer.succeed(None)
    else:
        w = Window(reactor)
        w.submitTo(Controller())
        d = w.go()
        return d

if __name__ == '__main__':
    react(reactor, main, sys.argv)
