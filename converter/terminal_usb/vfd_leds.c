#ifdef VFD_ENABLE

#include "hook.h"
#include "host.h"
#include "matrix.h"
#include "suspend.h"

#include "hd44780/hd44780.h"
#include "vfd_leds.h"

uint8_t last_led_status;

void clear_led_row() {
  hd44780_setcursor(0, 0);
  hd44780_print("\x94               ");
}

void vfd_init() {
  hd44780_init();
  clear_led_row();
}

void hook_late_init() {
  vfd_init();
  last_led_status = host_keyboard_leds();
}

struct {
  uint8_t bit;
  uint8_t lcd_col;
  char *text_off;
  char *text_on;
} leds_cols[] = {
    {.bit = (1 << USB_LED_NUM_LOCK), .lcd_col = COL_NUM_LOCK, .text_off = "   ", .text_on = "NUM"},
    {.bit = (1 << USB_LED_CAPS_LOCK), .lcd_col = COL_CAPS_LOCK, .text_off = "    ", .text_on = "CAPS"},
    {.bit = (1 << USB_LED_SCROLL_LOCK), .lcd_col = COL_SCROLL_LOCK, .text_off = "    ", .text_on = "SCRL"},
};

void update_led_row(uint8_t led_status) {
  uint8_t led_diff = ~(last_led_status ^ led_status);

  for (uint8_t i = 0; i < sizeof(leds_cols) / sizeof(leds_cols[0]); i++) {
    hd44780_setcursor(leds_cols[i].lcd_col, 0);
    hd44780_print(led_status & leds_cols[i].bit ? leds_cols[i].text_on : leds_cols[i].text_off);
  }

  last_led_status = led_status;
}

void hook_keyboard_leds_change(uint8_t led_status) { update_led_row(led_status); }

static uint8_t keyboard_led_stats;
static uint8_t _led_stats;

void hook_usb_suspend_entry(void) {
  _led_stats = keyboard_led_stats;
  keyboard_led_stats = 0;

  matrix_clear();
  clear_keyboard();

  hd44780_command(HD44780_DISPLAYCONTROL | HD44780_DISPLAYOFF);
}

void hook_usb_wakeup(void) {
  suspend_wakeup_init();

  keyboard_led_stats = _led_stats;

  hd44780_command(HD44780_DISPLAYCONTROL | HD44780_DISPLAYON);
}

// TODO: this doesn't work
// void hook_usb_reset() {
//   clear_led_row();
//   update_led_row(host_keyboard_leds());
// }

#endif
