#ifdef VFD_ENABLE

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "../../tmk_core/protocol/lufa/descriptor.h"
#include "hd44780/hd44780.h"
#include "usb_cmd.h"

uint8_t vfd_usb_command = USB_CMD_NONE;
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
      case USB_CMD_NONE:
      default: // invalid command fallback
        vfd_usb_command = data_byte;
        break;

      case USB_CMD_SETCURSOR:
        vfd_usb_col = data_byte;
        vfd_usb_command = USB_CMD_NONE;
        break;

      case USB_CMD_WRITE: {
        static uint8_t write_count = 0;
        write_count = data_byte;
        vfd_usb_command = USB_CMD_WRITE_N;
        break;

        case USB_CMD_WRITE_N:
          vfd_usb_putchar(data_byte);
          write_count--;
          if (write_count == 0) {
            vfd_usb_command = USB_CMD_NONE;
          }
          break;
      }

      case USB_CMD_BRIGHTNESS:
        vfd_set_brightness(data_byte);
        vfd_usb_command = USB_CMD_NONE;
        break;

      case USB_CMD_DEFINECHAR: {
        static uint8_t char_id;
        static uint8_t char_rows[8];
        static uint8_t char_rows_idx = 0;

        char_id = data_byte;
        vfd_usb_command = USB_CMD_DEFINECHAR_ID;
        break;

        case USB_CMD_DEFINECHAR_ID:
          char_rows[char_rows_idx++] = data_byte;
          if (char_rows_idx >= 8) {
            vfd_usb_command = USB_CMD_NONE;
            char_rows_idx = 0;

            hd44780_define_char(char_id, char_rows);
          }
          break;
      }

      case USB_CMD_CLEAR:
        vfd_usb_col = 0;
        for (uint8_t i = 0; i < 16; i++) {
          vfd_usb_putchar(' ');
        }
        vfd_usb_command = USB_CMD_NONE;
        break;

      case USB_CMD_PROGRAM:
        cli();
        wdt_reset();

        hd44780_setcursor(0, 0);
        hd44780_print("Waiting for     "
                      "program...      ");

        _delay_ms(5);
        UDCON = 1;
        USBCON = (1 << FRZCLK);
        _delay_ms(15);
#if defined(__AVR_AT90USB162__)
        asm volatile("jmp 0x3E00");
#elif defined(__AVR_ATmega32U4__)
        asm volatile("jmp 0x7E00");
#elif defined(__AVR_AT90USB646__)
        asm volatile("jmp 0xFC00");
#elif defined(__AVR_AT90USB1286__)
        asm volatile("jmp 0x1FC00");
#endif
        __builtin_unreachable();
    }
  }

  Endpoint_ClearOUT();
}

void hook_main_loop() { vfd_usb_task(); }

#endif
