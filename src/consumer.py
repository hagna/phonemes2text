from decoder import FlushingDecoder, MbrolaDecoder, OSXSayDecoder, osx_say
from common import react
from twisted.internet.task import LoopingCall
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor, defer
from twisted.python.usage import Options, UsageError
import random
import sys

class Echo(DatagramProtocol):

    def __init__(self, decoder=None):
        self.done = defer.Deferred()
        if decoder is None:
            f = MbrolaDecoder(0.500)
        else:
            f = decoder
        self.decoder = f.decoder
        def updateF():
            f.update(reactor.seconds())
        s = LoopingCall(updateF)
        s.start(0.04, now=False)


    def datagramReceived(self, data, (host, port)):
        #print "received %r from %s:%d" % (data, host, port)
        if data == 'done':
            self.done.callback(None)
        try:
            r = [int(k) for k in data.split(' ')]
            self.decoder(r)
        except Exception, e:
            print e
        #self.transport.write(data, (host, port))


class Quiz(Echo):

    quiz = ['r', 's', 't', 'd', 'n']

    def ask(self):
        self.answer = self.r.choice(self.quiz)
        osx_say(self.answer)


    def __init__(self, *a, **kw):
        Echo.__init__(self, *a, **kw)
        r = random.Random()
        self.question = LoopingCall(self.ask)
        self.question.start(1, now=False)


    def datagramReceived(self, data, (host, port)):
        #print "received %r from %s:%d" % (data, host, port)
        if data == 'done':
            self.done.callback(None)
        try:
            r = [int(k) for k in data.split(' ')]
            self.decoder(r)
        except Exception, e:
            print e
        #self.transport.write(data, (host, port))



class RunOptions(Options):
    def getSynopsis(self):
        return """port"""

    optFlags = [('multi', 'm', 'Enable multicast'),
                ('say', 's', 'Use say command (for osx)'),
                ('quiz', 'q', 'quiz mode')]


    def parseArgs(self, port):
        self.port = int(port)



class StartOptions(Options):
    def getSynopsis(self):
        return """Usage: %s""" % __file__

    subCommands = [
        ('udp', None, RunOptions, 'Receive phonemes from udp network.'),
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
    if command == 'udp':
        so = opt.subOptions
        if so['say']:
            if so['quiz']:
                e = Quiz(OSXSayDecoder(0.500))
                reactor.listenUDP(so.port, e)
                return e.done
            else:
                e = Echo(OSXSayDecoder(0.500))
                reactor.listenUDP(so.port, e)
                return e.done
        else:
            e = Echo()
            reactor.listenUDP(so.port, e)
            return e.done
    else:
        return defer.succeed(None)


if __name__ == '__main__':
    react(reactor, main, sys.argv)
