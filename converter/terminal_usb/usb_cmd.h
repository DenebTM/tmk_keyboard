#ifndef _USB_CMD_H
#define _USB_CMD_H

/**
 * VFD commands
 * implemented as a really bad state machine
 */

#define USB_CMD_NONE 0x00

/**
 * 0x01 <cursor X>
 */
#define USB_CMD_SETCURSOR 0x01

/**
 * 0x02 <N> <chars 1..N>
 */
#define USB_CMD_WRITE 0x02
#define USB_CMD_WRITE_N (USB_CMD_WRITE | 0x80)

/**
 * 0x03 <any byte>
 */
#define USB_CMD_CLEAR 0x03

/**
 * 0x04 <0-3>
 */
#define USB_CMD_BRIGHTNESS 0x04

/**
 * 0x05 <char id> <rows 1..8>
 */
#define USB_CMD_DEFINECHAR 0x05
#define USB_CMD_DEFINECHAR_ID (USB_CMD_DEFINECHAR | 0x80)

/**
 * 0x99 <any byte>
 */
#define USB_CMD_PROGRAM 0xff

#endif
