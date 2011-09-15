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

import os, sys
from steno import Keyer

from pygame.locals import *

os.environ['SDL_VIDEO_CENTERED'] = '1'
pygame.init()
pygame.fastevent.init()
event_poll = pygame.fastevent.poll
event_post = pygame.fastevent.post

if MIDI:
    device_id = 5
    pygame.midi.init()
    if device_id is None:
        input_id = pygame.midi.get_default_input_id()
    else:
        input_id = device_id

    print ("using input_id :%s:" % input_id)
    piano = pygame.midi.Input( input_id )



#wp51 colors
blue = (0,6,178)
gray = (170,170,170)
red = (167,2,0)
white = (255,255,255)

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
        

statusmsg = None

def update_status_msg(msg):
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
        statusmsg = text
    else:
        print msg

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


RHAND = [0,1,2,3,4,5]
LHAND = [6,7,8,9,10,11]


keyboardkeymap = {pygame.K_a:LHAND[5],
                 pygame.K_w:LHAND[4],
                 pygame.K_e:LHAND[3],
                 pygame.K_r:LHAND[2],
                 pygame.K_g:LHAND[1],
                 pygame.K_v:LHAND[0],
                 pygame.K_n:RHAND[0],
                 pygame.K_h:RHAND[1],
                 pygame.K_u:RHAND[2],
                 pygame.K_i:RHAND[3],
                 pygame.K_o:RHAND[4],
                 pygame.K_SEMICOLON:RHAND[5]}


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

def lookup(k, validkeys=keyboardkeymap):
    res = validkeys.get(k, None)
    return res


def lookup_piano(k):
    return lookup(k, pianokeymap)

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
                         ]).next


def load_keymap():
    try:
        fh = open('keymap.json', 'r')
        newone = json.load(fh)
        keyboardkeymap.clear()
        for i in newone:
            keyboardkeymap[int(i)] = newone[i]
    except Exception, e:
        print e


def dump_keymap():
    try:
        fh = open('keymap.json', 'w')
        json.dump(keyboardkeymap, fh)
    except Exception, e:
        print e

def updatekeymap(new, old):
    global keyboardkeymap
    keyboardkeymap.clear()
    res = {}
    j = range(RHAND[-1], RHAND[0]-1, -1) + range(LHAND[-1], LHAND[0]-1, -1)
    newmap = zip(new, j)
    for i,j in newmap:
        keyboardkeymap[i] = j

f = FlushingDecoder(500)
lkeyer = Keyer(f.decoder, 1500)
rkeyer = Keyer(f.decoder, 1500)
learning = False
listeners = []
load_keymap()
try:
    screen = pygame.display.set_mode((800, 600))
    pygame.display.set_caption('simplesteno')
    pygame.mouse.set_visible(0)
    background = pygame.Surface(screen.get_size())
    background = background.convert()
    background.fill(blue)
    helper_text()
    screen.blit(background, (0, 0))
    pygame.display.flip()
    clock = pygame.time.Clock()
    while True:
        screen.blit(background, (0, 0))
        pygame.display.flip()

        clock.tick(60)
        t = pygame.time.get_ticks()
        f.update(t)
        event = event_poll()
        if MIDI:
           if piano.poll():
                midi_events = piano.read(10)
                # convert them into pygame events.
                midi_evs = pygame.midi.midis2events(midi_events, piano.device_id)

                for m_e in midi_evs:
                    event_post( m_e )


        if event.type == pygame.QUIT:
            break
        if event.type in (KEYDOWN, KEYUP):
            key = event.key
            z = lookup(key)
            if event.type == KEYDOWN:
                if key == pygame.K_ESCAPE:
                    break
                if learning:
                    newkeymap.append(key)
                    if len(newkeymap) == 12:
                        learning = False
                        updatekeymap(newkeymap, keyboardkeymap)
                        update_status_msg('')
                        dump_keymap()
                        continue
                    update_status_msg(_learn_keymap()[0])
                if key == 284:
                    newkeymap = []
                    learning = True
                    update_status_msg(_learn_keymap()[0])
                    continue
                for m in listeners:
                    m(event)
                if z in RHAND:
                    rkeyer.keydown(z, t)
                if z in LHAND:
                    lkeyer.keydown(z-LHAND[0], t)
            if event.type == KEYUP:
                if z in RHAND:
                    rkeyer.keyup(z, t)
                if z in LHAND:
                    lkeyer.keyup(z-LHAND[0], t)
        if event.type == MIDIIN:
            note, vel = event.data1, event.data2
            z = lookup_piano(note)
            if vel > 0: # keydown

                if z is not None:
                    keyer.keydown(z, t)
            if vel == 0: # keyup
                if z is not None:
                    keyer.keyup(z, t)
finally:
    if MIDI:
        if piano:
            del piano
        pygame.midi.quit()
    pygame.quit()  # Keep this IDLE friendly 
