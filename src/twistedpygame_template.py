from twisted.internet import reactor
from twisted.internet.task import LoopingCall

import pygame
import pygame.font, pygame.time, pygame.fastevent, pygame.draw
import os, sys
from pygame.locals import *



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
        print event
        if event.type == pygame.locals.QUIT or \
                event.type == pygame.KEYDOWN and event.key == pygame.K_q:
            self.stop()
        elif self.controller is not None:
            if event.type == pygame.KEYDOWN:
                self.controller.keyDown(event.key)
            elif event.type == pygame.KEYUP:
                self.controller.keyUp(event.key)
            elif event.type == pygame.MOUSEMOTION:
                if pygame.event.get_grab():
                    self.controller.mouseMotion(
                        event.pos, event.rel, event.buttons)
            elif event.type == pygame.MOUSEBUTTONUP:
                pygame.event.set_grab(not pygame.event.get_grab())
                pygame.mouse.set_visible(not pygame.mouse.set_visible(True))


    def submitTo(self, controller):
        """
        Specify the given controller as the one to receive further
        events.
        """
        self.controller = controller


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



if __name__ == '__main__':

    w = Window(reactor)
    d = w.go()
    d.addBoth(lambda a: reactor.stop())
    reactor.run()
