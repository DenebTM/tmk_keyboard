import time
import usb.core
import sys

vid = 0xfeed
pid = 0x6535
ep_addr = 0x03


def main():
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        if dev is None:
            print(f'Could not find device {vid}:{pid}.')
            sys.exit(1)

        exit(1)

    c1 = [
        0b00000000,
        0b00001010,
        0b00000000,
        0b00010001,
        0b00010101,
        0b00001010,
        0b00000000,
        0b00000000
    ]
    cdef1 = ''.join([chr(cc) for cc in c1])
    c2 = [
        0b00000000,
        0b00001010,
        0b00000000,
        0b00010001,
        0b00010101,
        0b00001010,
        0b00000000,
        0b00011111
    ]
    cdef2 = ''.join([chr(cc) for cc in c2])

    dev.write(ep_addr, '\x01\x00')     # set cursor to 0

    packet = ''.join([
        '\x05\x00' + cdef1,             # define char
        '\x02\x08' + 8 * '\x00',        # print left half
        '\x01\x00',                     # reset cursor to 0
        '\x02\x08' + 8 * ' ',           # clear left half

        '\x05\x00' + cdef2,             # define char
        '\x02\x08' + 8 * '\x00',        # print right half
        '\x01\x08',                     # reset cursor to 8
        '\x02\x08' + 8 * ' ',           # clear right half
    ])

    n_iters = 10000
    start = time.time()
    for i in range(n_iters):
        dev.write(ep_addr, packet)

    d_t = time.time() - start
    bw = (len(packet) * n_iters) / d_t
    print('{:10.6}\tseconds'.format(d_t))
    print('{:10.6f}\tkB/s'.format(bw / 1000))


if __name__ == '__main__':
    main()
