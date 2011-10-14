from twisted.internet import protocol
from twisted.internet import reactor, utils
from twisted.python.filepath import FilePath
from twisted.python.procutils import which
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
        self.buf = []
        self.lastdecode = 0

    def flush(self):
        if self.buf:
            s = ''.join(self.buf)
            synth("[[" + s + "]]", phonemes=True)
            self.buf = []

    def update(self, ticks):
        self.ticks = ticks
        n = self.ticks - self.lastdecode
        if self.ticks - self.lastdecode > self.timeout:
            self.flush()

    def get_phone(self, a):
        return espeak_phonemes.get(a, None)

    def decoder(self, b):
        self.lastdecode = self.ticks
        l = list(b)
        l.sort()
        phone = self.get_phone(tuple(l))
        if synth is None or phone is None:
            print phone
        else:
            self.buf.append(phone)
            print self.buf



class MbrolaDecoder(FlushingDecoder):
    trans = {'I2':'I', '0':'O', 'a':'{', 'e':'E', 'i:':'i', 'eI':'EI',
             'u:':'u', 'aI':'AI', 'oU':'@U', '3:':'r=', 'ju:':['k', 'j', 'u'],
             'N':'Z', 'Z3':'Z'} 

    def espeak2mbrola(self, buf):
        res = []
        for i in buf:
            new = self.trans.get(i, i)
            if type(new) == type([]):
                res += new
            else:
                res.append(new)
        return res


    def flush(self):
        if self.buf:
            self.buf = self.espeak2mbrola(self.buf)
            mbrolaplay(self.buf)
            self.buf = []



class OSXSayDecoder(FlushingDecoder):

    phonemes = {(4,): 'n', # consonants
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
                    (1,2,3,4,5): 'y',
                    (2,5): 'C',
                    (1,4,5): 'J',
                    (1,2,4): 'T',
                    (1,3,4,5): 'Z',
                    (0,): 'AX', # vowels
                    (0,4): 'IX',
                    (0,2): 'AO',
                    (0,1): 'IH',
                    (0,3): 'AE',
                    (0,2,3,4): 'EH',
                    (0,2,3): 'IY',
                    (0,5): 'EY',
                    (0,3,4): 'UX',
                    (0,2,3,4,5): 'UW',
                    (0,4,5): 'AY',
                    (0,3,4,5): 'OW',
                    (0,2,5): None,
                    (0,2,3,5): 'AW',
                    (0,3,5): None,
                    (0,2,4,5): 'OY'}


    def get_phone(self, a):
        return self.phonemes.get(a, None)

    def flush(self):
        if self.buf:
            osx_say(self.buf)
            self.buf = []


        

class MyPP(protocol.ProcessProtocol):


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



class MbrolaPP(MyPP):
    aplay = '/usr/local/bin/aplay'

    def __init__(self, fp, buf):
        self.fp = fp
        self.data = ''
        self.phonemes = self.makephonemes(buf)

    def makephonemes(self, buf):
        res = [str(k) + ' 100' for k in buf]
        res = '\n'.join(res)
        print res
        return res

    def processEnded(self, reason):
        print "processEnded, status %d" % (reason.value.exitCode,)
        print "quitting"
        d = utils.getProcessOutput(self.aplay, (self.fp.path,), errortoo=True)
        d.addCallback(lambda a: self.fp.remove())

 

def osx_say(buf):
    processProtocol = MyPP()
    executable = which('say')
    args = [executable, '[[inpt PHO]] ' + ''.join(buf)]
    reactor.spawnProcess(processProtocol, executable, args=args,
                         env={'HOME': os.environ['HOME']})


def mbrolaplay(buf):
    fp = FilePath('.').temporarySibling('.wav')
    processProtocol = MbrolaPP(fp, buf)
    executable = '/home/tc/mbrola-linux-i386'
    program = executable
    args = [executable, '/home/tc/us1/us1', '-', fp.path]
    reactor.spawnProcess(processProtocol, executable, args=args,
                         env={'HOME': os.environ['HOME']})



if __name__ == '__main__':
    osx_say(['h','EH', 'l', 'AW'])
    reactor.run()
    #./mbrola-linux-i386 us1/us1 us1/TEST/xmas.pho test.wav
