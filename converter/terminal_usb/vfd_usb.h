#ifndef _VFD_USB_H
#define _VFD_USB_H

/**
 * VFD commands
 * implemented as a really bad state machine
 */

#define VFD_CMD_NONE 0x00

/**
 * 0x01 <cursor X>
 */
#define VFD_CMD_SETCURSOR 0x01

/**
 * 0x02 <N> <char #1/N ... #N/N>
 */
#define VFD_CMD_WRITE 0x02
#define VFD_CMD_WRITE_N (VFD_CMD_WRITE | 0x80)

/**
 * 0x03 <any byte>
 */
#define VFD_CMD_CLEAR 0x03

/**
 * 0x04 <0-3>
 */
#define VFD_CMD_BRIGHTNESS 0x04

/**
 * TODO
 * 0x0f <char id> <character definition>
 */
#define VFD_CMD_DEFINECHAR 0x0f

#endif
