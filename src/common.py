
from twisted.internet import reactor
from twisted.python import log

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
