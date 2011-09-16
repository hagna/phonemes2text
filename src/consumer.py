from decoder import FlushingDecoder
from common import react
from twisted.internet.task import LoopingCall
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor, defer
from twisted.python.usage import Options, UsageError
import sys

class Echo(DatagramProtocol):

    def __init__(self):
        self.done = defer.Deferred()
        f = FlushingDecoder(0.500)
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


class RunOptions(Options):
    def getSynopsis(self):
        return """port"""

    optFlags = [('multi', 'm', 'Enable multicast')]


    def parseArgs(self, port):
        self.port = int(port)



class StartOptions(Options):
    def getSynopsis(self):
        return """Usage: %s""" % __file__

    subCommands = [
        ('udp', None, RunOptions, 'Send phonemes over the network.'),
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
        e = Echo()
        reactor.listenUDP(so.port, e)
        return e.done


if __name__ == '__main__':
    react(reactor, main, sys.argv)