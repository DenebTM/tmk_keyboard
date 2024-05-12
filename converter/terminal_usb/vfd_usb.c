#ifdef VFD_ENABLE

#include "vfd_usb.h"
#include "../../tmk_core/protocol/lufa/descriptor.h"
#include "hd44780/hd44780.h"

uint8_t vfd_usb_command = VFD_CMD_NONE;
uint8_t vfd_usb_col = 0;

/**
 * print a character to the display on line 2 and advance cursor
 *
 * wrap around to beginning at column 16
 */
void vfd_usb_putchar(uint8_t data) {
  hd44780_setcursor(vfd_usb_col++, 1);
  hd44780_putchar(data);
}

/**
 * handle commands sent via USB interface
 */
void vfd_usb_task() {
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  Endpoint_SelectEndpoint(VFD_OUT_EPNUM);

  uint16_t avail_bytes = Endpoint_BytesInEndpoint();
  while (avail_bytes-- > 0) {
    uint8_t data_byte = (uint8_t)Endpoint_Read_8();

    switch (vfd_usb_command) {
      case VFD_CMD_NONE:
      default: // invalid command fallback
        vfd_usb_command = data_byte;
        break;

      case VFD_CMD_SETCURSOR:
        vfd_usb_col = data_byte;
        vfd_usb_command = VFD_CMD_NONE;
        break;

      case VFD_CMD_WRITE: {
        static uint8_t write_count = 0;
        write_count = data_byte;
        vfd_usb_command = VFD_CMD_WRITE_N;
        break;

        case VFD_CMD_WRITE_N:
          vfd_usb_putchar(data_byte);
          write_count--;
          if (write_count == 0) {
            vfd_usb_command = VFD_CMD_NONE;
          }
          break;
      }

      case VFD_CMD_BRIGHTNESS:
        vfd_set_brightness(data_byte);
        vfd_usb_command = VFD_CMD_NONE;
        break;

      case VFD_CMD_DEFINECHAR: {
        static uint8_t char_id;
        static uint8_t char_rows[8];
        static uint8_t char_rows_idx = 0;

        char_id = data_byte;
        vfd_usb_command = VFD_CMD_DEFINECHAR_ID;
        break;

        case VFD_CMD_DEFINECHAR_ID:
          char_rows[char_rows_idx++] = data_byte;
          if (char_rows_idx >= 8) {
            vfd_usb_command = VFD_CMD_NONE;
            char_rows_idx = 0;

            hd44780_define_char(char_id, char_rows);
          }
          break;
      }

      case VFD_CMD_CLEAR:
        vfd_usb_col = 0;
        for (uint8_t i = 0; i < 16; i++) {
          vfd_usb_putchar(' ');
        }
        vfd_usb_command = VFD_CMD_NONE;
        break;
    }
  }

  Endpoint_ClearOUT();
}

void hook_main_loop() { vfd_usb_task(); }

#endif
