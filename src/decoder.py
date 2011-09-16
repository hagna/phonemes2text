from twisted.internet import protocol
from twisted.internet import reactor, utils
from twisted.python.filepath import FilePath
import re, os

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



class MbrolaDecoder(FlushingDecoder):
    def flush(self):
        if self.buffer:
            mbrolaplay(self.buffer)
            self.buffer = []

        

class MyPP(protocol.ProcessProtocol):

    aplay = '/usr/local/bin/aplay'

    def __init__(self, fp, buffer):
        self.fp = fp
        self.data = ''
        self.phonemes = self.makephonemes(buffer)

    def makephonemes(self, buffer):
        res = [str(k) + ' 100' for k in buffer]
        res = '\n'.join(res)
        print res
        return res


    def connectionMade(self):
        print "connectionMade!"
        self.transport.write(self.phonemes)
        self.transport.closeStdin() # tell them we're done
    def outReceived(self, data):
        print "outReceived! with %d bytes!" % len(data)
        self.data = self.data + data
    def errReceived(self, data):
        print "errReceived! with %d bytes!" % len(data)
        print data
    def inConnectionLost(self):
        print "inConnectionLost! stdin is closed! (we probably did it)"
    def outConnectionLost(self):
        print "outConnectionLost! The child closed their stdout!"
        # now is the time to examine what they wrote
        #print "I saw them write:", self.data
        print "I saw %s lines" % self.data
    def errConnectionLost(self):
        print "errConnectionLost! The child closed their stderr."
    def processExited(self, reason):
        print "processExited, status %d" % (reason.value.exitCode,)
    def processEnded(self, reason):
        print "processEnded, status %d" % (reason.value.exitCode,)
        print "quitting"
        d = utils.getProcessOutput(self.aplay, (self.fp.path,), errortoo=True)
        d.addCallback(lambda a: self.fp.remove())


def mbrolaplay(buffer):
    fp = FilePath('.').temporarySibling('.wav')
    processProtocol = MyPP(fp, buffer)
    executable = '/home/tc/mbrola-linux-i386'
    program = executable
    args = [executable, '/home/tc/us1/us1', '-', fp.path]
    reactor.spawnProcess(processProtocol, executable, args=args,
                         env={'HOME': os.environ['HOME']})


if __name__ == '__main__':
    mbrolaplay(['n', 'EI', 't', 'EI'])
    reactor.run()
    #./mbrola-linux-i386 us1/us1 us1/TEST/xmas.pho test.wav
