from twisted.trial import unittest
from steno import Keyer


class TestKeyer(unittest.TestCase):
    def setUp(self):
        self.k = Keyer(None)


    def test_keyup_basic(self):
        """
        Keyer.keyup adds the key to the buffer and sets max to the
        key's timestamp.
        """
        self.k.keyup('A', 1000)
        self.assertEquals(self.k.maxtime, 1000)
        self.assertEquals(self.k.buffer, ['A'])


    def test_keyup_again(self):
        """
        Keyer.keyup appends the key to the buffer and alters maxtime
        if timestamp is greater than the old maxtime.
        """
        self.k.keyup('A', 1000)
        self.k.keyup('B', 1001)
        self.k.keyup('C', 1002)
        self.assertEquals(self.k.maxtime, 1002)
        self.assertEquals(self.k.buffer, ['A', 'B', 'C'])


    def test_keydown_basic(self):
        """
        Keyer.keydown appends to a keydownbuffer.
        """
        self.k.keydown('A', 900)
        self.k.keydown('B', 901)
        self.assertEquals(self.k.keydownbuffer, ['A', 'B'])


    def test_all_keys_up(self):
        """
        Keyer.all_keys_up is True if keydownbuffer is empty.
        """
        self.k.keydownbuffer = [1,2,3]
        self.assertEquals(self.k.all_keys_up(), False)
        self.k.keydownbuffer = []
        self.assertEquals(self.k.all_keys_up(), True)


    def test_keyup_removes_keydown(self):
        """
        Keyer.keyup removes the corresponding key from keydownbuffer.
        """
        self.k.keydownbuffer = [1,2]
        self.k.keyup(1, 1000)
        self.assertEquals(self.k.keydownbuffer, [2])


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
        Keyer.keyup removes everything in the buffer if timestamp is
        greater maxtime by the threshold amount. 
        """
        self.k.threshold = 1
        self.k.keyup('A', 1000)
        self.k.keyup('CC', 1002)
        self.assertEquals(self.k.buffer, ['CC'])


    def test_under_threshold(self):
        """
        otherwise leaves it alone.
        """
        self.k.threshold = 99
        self.k.keyup('A', 1000)
        self.k.keyup('CC', 1099)
        self.assertEquals(self.k.buffer, ['A', 'CC'])


    def test_keyup_calls_decoder(self):
        """
        Keyer.keyup calls decoder when all the keys are up.
        """
        calls = []
        def fake(t):
            calls.append(t)
        self.k = Keyer(fake)
        self.k.keydown('A', 12000)
        self.k.keydown('B', 1010101)
        self.k.keyup('A', 10101)
        self.k.keyup('B', 10102)
        self.assertEquals(calls, [('A','B')])
