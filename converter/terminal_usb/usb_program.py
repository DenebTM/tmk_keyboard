import usb.core

# Bulk Vendor HID device VID and PID
vid = 0xfeed
pid = 0x6535
ep_addr = 0x03

cmd_program = b'\xff\x00'


def main():
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print(f'Could not find device {vid}:{pid}.')
        sys.exit(1)

    dev.write(ep_addr, cmd_program)


if __name__ == '__main__':
    main()
