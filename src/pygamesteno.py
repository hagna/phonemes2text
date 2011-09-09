import pygame
import pygame.font, pygame.time
import os, sys
from steno import Keyer


from pygame.locals import *

os.environ['SDL_VIDEO_CENTERED'] = '1'
pygame.init()


def helper_text():
    if pygame.font:
        font = pygame.font.Font(None, 36)
        text = font.render("steno", 1, (10, 10, 10))
        textpos = text.get_rect(centerx=background.get_width()/2)
        background.blit(text, textpos)
    else:
        print "sorry no font for you"

try:
    from espeak import espeak 
    synth = espeak.synth
    espeak.set_parameter(espeak.Parameter.Rate, 1)
    espeak.set_parameter(espeak.Parameter.Volume, 100)
    espeak.synth("hello there")
    #print espeak.get_parameter("rate")
except Exception, e:
    print e
    print "no espeak"
    synth = None

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
            print "flush", s
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


validkeys = {pygame.K_q:5,
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

def lookup(k):
    res = validkeys.get(k, None)
    return res

f = FlushingDecoder(500)
keyer = Keyer(f.decoder, 1500)

try:
    screen = pygame.display.set_mode((800, 600))
    pygame.display.set_caption('simplesteno')
    pygame.mouse.set_visible(0)
    background = pygame.Surface(screen.get_size())
    background = background.convert()
    background.fill((250, 250, 250))
    helper_text()
    screen.blit(background, (0, 0))
    pygame.display.flip()
    clock = pygame.time.Clock()
    while True:
        clock.tick(60)
        t = pygame.time.get_ticks()
        f.update(t)
        event = pygame.event.poll()
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
finally:
    pygame.quit()  # Keep this IDLE friendly 
