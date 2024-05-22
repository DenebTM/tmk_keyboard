import sys
import tty
from time import sleep
import usb.core


class VFDUSB:
    _cmd_write = b'\x02'
    _cmd_clear = b'\x03\x00'
    _cmd_setcursor = b'\x01'
    _cmd_brightness = b'\x04'
    _cmd_definechar = b'\x05'
    _ep_addr = 0x03
    _vid = 0xfeed
    _pid = 0x6535

    dev = None

    def __init__(self):
        self.dev = usb.core.find(idVendor=self._vid, idProduct=self._pid)
        if self.dev is None:
            print(f'Could not find device {self._vid}:{self._pid}.')
            raise FileNotFoundError

    def clear(self):
        self.dev.write(self._ep_addr, self._cmd_clear)

    def set_brightness(self, val):
        self.dev.write(self._ep_addr,
                       self._cmd_brightness + int(val % 4).to_bytes(1))

    def set_cursor(self, x):
        self.dev.write(self._ep_addr,
                       self._cmd_setcursor + int(x % 16).to_bytes(1))

    def puts(self, string):
        if len(string) >= 256:
            self.puts(string[:256])
            self.puts(string[256:])
            return
        self.dev.write(self._ep_addr,
                       self._cmd_write + len(string).to_bytes(1)
                       + string.encode())

    def puts_at(self, x, string):
        self.set_cursor(x)
        self.puts(string)

    def define_char(self, i, lines):
        self.dev.write(self._ep_addr,
                       self._cmd_definechar + int(i % 8).to_bytes(1) + lines)


def main():
    dev = VFDUSB()

    chars = [
        b'\x10\x00\x00\x00\x00\x00\x00\x00',
        b'\x18\x00\x00\x00\x00\x00\x00\x00',
        b'\x1c\x00\x00\x00\x00\x00\x00\x00',
        b'\x1e\x00\x00\x00\x00\x00\x00\x00',
        b'\x0f\x00\x00\x00\x00\x00\x00\x00',
        b'\x07\x00\x00\x00\x00\x00\x00\x00',
        b'\x03\x00\x00\x00\x00\x00\x00\x00',
        b'\x01\x00\x00\x00\x00\x00\x00\x00'
    ]

    chidx = [' '] + [i.to_bytes(1).decode() for i in range(8)]
    chtup = [
        (0, 0),
        (1, 0),
        (2, 0),
        (3, 0),
        (4, 0),
        (5, 0),
        (6, 1),
        (7, 2),
        (8, 3),
        (0, 4),
        (0, 5),
    ]
    restart_idx = 6

    dev.clear()
    for i, char in enumerate(chars):
        dev.define_char(i, char)

    cur = 0
    idx = 0
    while True:
        while idx < len(chtup):
            _1, _2 = chtup[idx]
            dev.puts_at(cur, chidx[_1] + chidx[_2])
            sleep(0.2)
            idx += 1
        idx = restart_idx
        cur += 1


if __name__ == '__main__':
    main()
