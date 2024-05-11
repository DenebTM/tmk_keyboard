#ifndef _HD44780_H
#define _HD44780_H

#include <avr/io.h>
#include <stdbool.h>

#define BIT0 (1 << 0)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)

#define HD44780_DATA PORTB
#define HD44780_DATA_DDR DDRB

#define HD44780_CTRL PORTF
#define HD44780_CTRL_DDR DDRF

#define HD44780_RS BIT5
#define HD44780_RW BIT6
#define HD44780_EN BIT7

/**
 * from Arduino LiquidCrystal library
 */
// commands
#define HD44780_CLEARDISPLAY 0x01
#define HD44780_RETURNHOME 0x02
#define HD44780_ENTRYMODESET 0x04
#define HD44780_DISPLAYCONTROL 0x08
#define HD44780_CURSORSHIFT 0x10
#define HD44780_FUNCTIONSET 0x20
#define HD44780_SETCGRAMADDR 0x40
#define HD44780_SETDDRAMADDR 0x80

// flags for display entry mode
#define HD44780_ENTRYRIGHT 0x00
#define HD44780_ENTRYLEFT 0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define HD44780_DISPLAYON 0x04
#define HD44780_DISPLAYOFF 0x00
#define HD44780_CURSORON 0x02
#define HD44780_CURSOROFF 0x00
#define HD44780_BLINKON 0x01
#define HD44780_BLINKOFF 0x00

// flags for display/cursor shift
#define HD44780_DISPLAYMOVE 0x08
#define HD44780_CURSORMOVE 0x00
#define HD44780_MOVERIGHT 0x04
#define HD44780_MOVELEFT 0x00

// flags for function set
#define HD44780_8BITMODE 0x10
#define HD44780_4BITMODE 0x00
#define HD44780_2LINE 0x08
#define HD44780_1LINE 0x00
#define HD44780_5x10DOTS 0x04
#define HD44780_5x8DOTS 0x00

void hd44780_init();

void hd44780_send(uint8_t data, bool rs);
void hd44780_command(uint8_t data);
void hd44780_write(uint8_t data);

void hd44780_clear();
void hd44780_putchar(char c);
void hd44780_print(char *s);
void hd44780_setcursor(uint8_t col, uint8_t row);

void vfd_set_brightness(uint8_t brightness);

#endif
