import sys
from time import sleep
import usb.core
import usb.util

# Bulk Vendor HID device VID and PID
vid = 0xfeed
pid = 0x6535
ep_addr = 0x03

cmd_write = b'\x02\x01'
cmd_clear = b'\x03\x00'
cmd_setcursor = b'\x01\x00'
cmd_brightness = b'\x04'


def main():
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print(f'Could not find device {vid}:{pid}.')
        sys.exit(1)

    dev.write(ep_addr, cmd_clear)
    sleep(0.25)

    ch = 0
    br = 3
    while True:
        dev.write(ep_addr, cmd_brightness + br.to_bytes(1))
        br = 3 if br == 0 else 0
        # dev.write(ep_addr, cmd_write + ch.to_bytes(1))
        # ch = (ch + 1) % 256
        sleep(0.25)


if __name__ == '__main__':
    main()
