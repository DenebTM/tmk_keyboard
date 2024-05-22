#include "hd44780.h"
#include "timer.h"

#include <avr/io.h>
#include <util/delay.h>

#ifndef LCD_COLS
#define LCD_COLS 16
#endif
#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif

bool update_cursor = true;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

bool hd44780_initialized = false;

void function_set(uint8_t brightness) {
  hd44780_command(HD44780_FUNCTIONSET | HD44780_8BITMODE | HD44780_2LINE | HD44780_5x8DOTS | brightness);
  _delay_ms(4.5);
}

void hd44780_init() {
  // set control pins as output
  HD44780_CTRL_DDR |= _BV(HD44780_RS) | _BV(HD44780_RW) | _BV(HD44780_EN);

  // set data pins as output
  HD44780_DATA_OUT = 0xff;
  HD44780_DATA_DDR = 0xff;

  // initialize control lines and output
  HD44780_CTRL &= ~(_BV(HD44780_RS) | _BV(HD44780_RW) | _BV(HD44780_EN));
  HD44780_DATA_OUT = 0x00;

  // turn off output comparators 0A and 1C, as they override PORTB7 input
  // (LCD/VFD "Busy" pin)
  TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));
  TCCR1A &= ~(_BV(COM1C0) | _BV(COM1C1));

  // wait for voltage to settle (TODO check how long this actually needs to be)
  _delay_ms(50);

  function_set(0);

  hd44780_command(HD44780_DISPLAYCONTROL | HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF);

  hd44780_clear();
  update_cursor = true;
  cursor_x = 0;
  cursor_y = 0;
  hd44780_initialized = true;
}

void hd44780_send(uint8_t data, bool rs) {
  // wait until not busy
  if (hd44780_initialized) {
    // set data register as input
    HD44780_DATA_DDR = 0;
    HD44780_DATA_IN = 0xff;

    HD44780_CTRL &= ~(_BV(HD44780_RS) | _BV(HD44780_EN));
    HD44780_CTRL |= _BV(HD44780_RW);

    uint16_t timeout_start = timer_read();
    static bool timeout = false;
    bool busy = true;

    while (busy && !timeout) {
      HD44780_CTRL |= _BV(HD44780_EN);
      _delay_us(0.5);

      busy = HD44780_DATA_IN >> 7;

      HD44780_CTRL &= ~_BV(HD44780_EN);
      _delay_us(0.5);

      timeout = (timer_read() - timeout_start) >= 10000;
    }
  }

  // setup for data write
  HD44780_DATA_DDR = 0xff;
  HD44780_CTRL &= ~(_BV(HD44780_RS) | _BV(HD44780_RW));
  HD44780_CTRL |= (rs * _BV(HD44780_RS));

  HD44780_DATA_OUT = data;

  // pulse enable pin
  HD44780_CTRL |= _BV(HD44780_EN);
  _delay_us(0.5);
  HD44780_CTRL &= ~_BV(HD44780_EN);
  _delay_us(0.5);
}

void hd44780_command(uint8_t data) { hd44780_send(data, false); }
void hd44780_write(uint8_t data) { hd44780_send(data, true); }

void hd44780_putchar(char c) {
  hd44780_write(c);
  if (++cursor_x >= LCD_COLS) {
    cursor_y = (cursor_y + 1) % LCD_ROWS;
    cursor_x = 0;
    hd44780_setcursor(cursor_x, cursor_y);
  }
}

void hd44780_clear() { hd44780_command(HD44780_CLEARDISPLAY); }

void hd44780_print(char *s) {
  // ensure that we are writing to DDRAM
  hd44780_setcursor(cursor_x, cursor_y);

  for (int i = 0; s[i] != 0; i++) {
    hd44780_putchar(s[i]);
  }
}

void hd44780_setcursor(uint8_t col, uint8_t row) {
  cursor_x = col % LCD_COLS;
  cursor_y = row % LCD_ROWS;
  hd44780_command(HD44780_SETDDRAMADDR | (cursor_y * 64 + cursor_x));
}

/**
 * set brightness level from 0 (highest) to 3 (lowest)
 *
 * should work on both Noritake and Futaba VFDs
 */
void vfd_set_brightness(uint8_t brightness) {
  brightness &= 0x03;

  // ensure any garbage gets written off-screen, at least on Futaba 16x2 displays
  hd44780_command(HD44780_SETDDRAMADDR | LCD_COLS);

  // for Futaba (function set including brightness)
  function_set(brightness);

  // for Noritake (brightness after function set)
  hd44780_write(brightness);

  // restore cursor position
  hd44780_setcursor(cursor_x, cursor_y);
}

void hd44780_define_char(uint8_t id, uint8_t rows[8]) {
  id &= 0x7;
  hd44780_command(HD44780_SETCGRAMADDR | (id << 3));
  for (int i = 0; i < 8; i++) {
    hd44780_write(rows[i]);
  }
}
