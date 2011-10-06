

code = '''\
    switch ( buf ) {
       case 0b00010000:
            Buffer_StoreElement(&steno_buffer, HID_KEYBOARD_SC_N);
            break;
       default:
            Buffer_StoreElement(&steno_buffer, HID_KEYBOARD_SC_U);
            Buffer_StoreElement(&steno_buffer, HID_KEYBOARD_SC_N);
            Buffer_StoreElement(&steno_buffer, HID_KEYBOARD_SC_K);

            break;
''' 


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

for d in [espeak_consonants, espeak_vowels]:
    for k in d.keys():
        b = [0] * 8
        for i in k:
            b[i] = 1
        b.reverse()
        b = '0b' + str(''.join([str(z) for z in b]))
        v = d[k]
        print ' ' * 8 + "case %s:" % b
        for ch in v:
            if ch.isupper():
                print ' ' * 12 + 'Buffer_StoreElement(&steno_buffer, 0x%x);' % (8192 + (ord(ch) - 61))
            else:
                print ' ' * 12 + 'Buffer_StoreElement(&steno_buffer, 0x%x);' % (ord(ch) - 93)
        print ' ' * 12 + 'break;'
