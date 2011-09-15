from twisted.trial import unittest
from steno import Keyer


class TestKeyer(unittest.TestCase):
    def setUp(self):
        self.k = Keyer(None)
        self.k.all_keys_up = lambda: False


    def test_keyUp_basic(self):
        """
        Keyer.keyUp adds the key to the buffer and sets max to the
        key's timestamp.
        """
        self.k.keyUp('A', 1000)
        self.assertEquals(self.k.maxtime, 1000)
        self.assertEquals(self.k.buffer, ['A'])


    def test_keyUp_again(self):
        """
        Keyer.keyUp appends the key to the buffer and alters maxtime
        if timestamp is greater than the old maxtime.
        """
        self.k.keyUp('A', 1000)
        self.k.keyUp('B', 1001)
        self.k.keyUp('C', 1002)
        self.assertEquals(self.k.maxtime, 1002)
        self.assertEquals(self.k.buffer, ['A', 'B', 'C'])


    def test_keyDown_basic(self):
        """
        Keyer.keyDown appends to a keyDownbuffer.
        """
        self.k.keyDown('A', 1001)
        self.k.keyDown('B', 1002)
        self.assertEquals(self.k.keyDownbuffer, ['A', 'B'])


    def test_all_keys_up(self):
        """
        Keyer.all_keys_up is True if keyDownbuffer is empty.
        """
        self.k = Keyer(None)
        self.k.keyDownbuffer = [1,2,3]
        self.assertEquals(self.k.all_keys_up(), False)
        self.k.keyDownbuffer = []
        self.assertEquals(self.k.all_keys_up(), True)


    def test_keyUp_removes_keyDown(self):
        """
        Keyer.keyUp removes the corresponding key from keyDownbuffer.
        """
        self.k.keyDownbuffer = [1,2]
        self.k.keyUp(1, 1000)
        self.assertEquals(self.k.keyDownbuffer, [2])


    def test_threshold_init(self):
        """
        Keyer.__init__ takes an argument for threshold value or uses a default.
        """
        k = Keyer(None, threshold=0)
        self.assertEquals(k.threshold, 0)
        k = Keyer(None)
        self.assertNotEquals(k.threshold, None)


    def test_gt_threshold_removes_buffer(self):
        """
        Keyer.keyUp removes everything in the buffer if timestamp is
        greater maxtime by the threshold amount. 
        """
        self.k.threshold = 1
        self.k.keyUp('A', 1000)
        self.k.keyUp('CC', 1002)
        self.assertEquals(self.k.buffer, ['CC'])


    def test_under_threshold(self):
        """
        otherwise leaves it alone.
        """
        self.k.threshold = 99
        self.k.keyUp('A', 1000)
        self.k.keyUp('CC', 1099)
        self.assertEquals(self.k.buffer, ['A', 'CC'])


    def test_keyUp_calls_decoder(self):
        """
        Keyer.keyUp calls decoder when all the keys are up and it
        clears the buffer.
        """
        calls = []
        def fake(t):
            calls.append(t)
        self.k = Keyer(fake)
        self.k.keyDown('A', 12000)
        self.k.keyDown('B', 1010101)
        self.k.keyUp('A', 10101)
        self.k.keyUp('B', 10102)
        self.assertEquals(calls, [('A','B')])
        self.assertEquals(self.k.buffer, [])
