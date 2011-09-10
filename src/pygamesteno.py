import pygame
import pygame.font, pygame.time, pygame.fastevent, pygame.midi
from pygame.midi import MIDIIN
import os, sys
from steno import Keyer

from pygame.locals import *

MIDI=False
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


def helper_text():
    if pygame.font:
        font = pygame.font.Font(None, 26)
        consonants = ['net 4', 'tin 3',
                       'r run 1',
                       's sin 2',
                       'd din 5',
                       'l let 1,4',
                       'th this 2,3',
                       'z zen 3,4',
                       'm met 1,2',
                       'k kin 2,3,4',
                       'v van 1,3',
                       'w win 1,2,3,4',
                       'p pin 1,2,3',
                       'f fin 1,5',
                       'b bin 4,5',
                       'h hen 2,4',
                       'ng sing 2,3,4,5',
                       'sh shin 1,3,4',
                       'g gift 3,4,5',
                       'y yes 1,2,3,4,5',
                       'ch chin 2,5',
                       'j jam 1,4,5',
                       'th thin 1,2,4',
                       'zh azure 1,3,4,5']
        vowels = ['a about 0',
                  'is Dennis 0,4',
                  'o bob 0,2',
                  'i bit 0,1',
                  'a bat 0,3',
                  'e bet 0,2,3,4', 
                  'ea beat 0,2,3',
                  'a bake 0,5',
                  'uh but 0,3,4',
                  'u lute 0,2,3,4,5',
                  'i bite 0,4,5',
                  'oa boat 0,2,4',
                  'oo book 0,3,4,5',
                  'i bird 0,2,5',
                  'ou bout 0,2,3,5',
                  'yu cute 0,3,5',
                  'oy boy 0,2,4,5']
        for k, phonemeset in enumerate([consonants, vowels]):
            for i, s in enumerate(phonemeset):
                text = font.render(s, 1, gray)
                textpos = text.get_rect(left=10+k*200,
                                        bottom=30+i*20)
                background.blit(text, textpos)
    else:
        print "no font for you"

try:
    from espeak import espeak 
    synth = espeak.synth
    espeak.set_parameter(espeak.Parameter.Rate, 100)
    espeak.set_parameter(espeak.Parameter.Volume, 100)
    espeak.set_voice('english-us')
    espeak.synth("hello")
    #print espeak.get_parameter("rate")
except Exception, e:
    print e
    print "no espeak"
    synth = None

import itertools
newvoice = itertools.cycle(['en-scottish', 'english', 'lancashire', 'english_rp', 'english_wmids', 'english-us', 'en-westindies']).next

espeak_phonemes = {(4,): 'n', # consonants
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
                   (1,3,4,5): 'Z3',
                   (0,): '@', # vowels
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


keyboardkeymap = {pygame.K_q:5,
                 pygame.K_2:4,
                 pygame.K_3:3,
                 pygame.K_r:2,
                 pygame.K_v:1,
                 pygame.K_SPACE:0,
                 pygame.K_m:1,
                 pygame.K_i:2,
                 pygame.K_0:3,
                 pygame.K_MINUS:4,
                 pygame.K_RIGHTBRACKET:5}


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


f = FlushingDecoder(500)
keyer = Keyer(f.decoder, 1500)

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
                if z is not None:
                    keyer.keydown(z, t)
            if event.type == KEYUP:
                if z is not None:
                    keyer.keyup(z, t)
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
