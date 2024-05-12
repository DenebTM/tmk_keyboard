import sys
import tty
from time import sleep
import usb.core
import usb.util

# Bulk Vendor HID device VID and PID
vid = 0xfeed
pid = 0x6535
ep_addr = 0x03

cmd_write = b'\x02\x01'
cmd_clear = b'\x03\x00'
cmd_setcursor = b'\x01'
cmd_brightness = b'\x04'


def main():
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print(f'Could not find device {vid}:{pid}.')
        sys.exit(1)

    tty.setraw(sys.stdin.fileno())
    dev.write(ep_addr, cmd_clear)

    cur = 0
    while True:
        ch = sys.stdin.read(1).encode()
        if ch[0] in [3, 4]:
            break

        if ch[0] in [ord('\n'), ord('\r')]:
            dev.write(ep_addr, cmd_clear)
            cur = 0
        elif ch[0] == 127:
            cur = (cur + 15) % 16
            dev.write(ep_addr, cmd_setcursor + chr(ord('0') + cur).encode())
            dev.write(ep_addr, cmd_write + ' '.encode())
            dev.write(ep_addr, cmd_setcursor + chr(ord('0') + cur).encode())
        else:
            dev.write(ep_addr, cmd_write + ch)
            cur = (cur + 1) % 16


if __name__ == '__main__':
    main()
