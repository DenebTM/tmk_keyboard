#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "matrix.h"
#include "action.h"
#include "backlight.h"
#include "suspend_avr.h"
#include "suspend.h"
#include "timer.h"
#ifdef PROTOCOL_LUFA
#include "lufa.h"
#endif


/* Power down MCU with watchdog timer
 * wdto: watchdog timer timeout defined in <avr/wdt.h>
 *          WDTO_15MS
 *          WDTO_30MS
 *          WDTO_60MS
 *          WDTO_120MS
 *          WDTO_250MS
 *          WDTO_500MS
 *          WDTO_1S
 *          WDTO_2S
 *          WDTO_4S
 *          WDTO_8S
 */
static uint8_t wdt_timeout = 0;
static void power_down(uint8_t wdto)
{
#ifdef PROTOCOL_LUFA
    if (USB_DeviceState == DEVICE_STATE_Configured) return;
#endif
    wdt_timeout = wdto;

    // Watchdog Interrupt Mode
    wdt_intr_enable(wdto);

    // TODO: more power saving
    // See PicoPower application note
    // - I/O port input with pullup
    // - prescale clock
    // - BOD disable
    // - Power Reduction Register PRR
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();

    // Disable watchdog after sleep
    wdt_disable();
}

#ifdef SUSPEND_MODE_STANDBY
static void standby(void)
{
#ifdef SLEEP_MODE_STANDBY
    set_sleep_mode(SLEEP_MODE_STANDBY);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
#endif
}
#endif

static void idle(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}


void suspend_idle(uint8_t time)
{
    idle();
}

void suspend_power_down(void)
{
#ifdef NO_SUSPEND_POWER_DOWN
    ;
#elif defined(SUSPEND_MODE_NOPOWERSAVE)
    ;
#elif defined(SUSPEND_MODE_STANDBY)
    standby();
#elif defined(SUSPEND_MODE_IDLE)
    idle();
#else
    power_down(WDTO_15MS);
#endif
}

bool suspend_wakeup_condition(void)
{
#ifndef NO_KEYBOARD
    matrix_power_up();
    matrix_scan();
    matrix_power_down();
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (matrix_get_row(r)) return true;
    }
#endif
    return false;
}

// run immediately after wakeup
void suspend_wakeup_init(void)
{
#ifndef NO_KEYBOARD
    // clear keyboard state
    matrix_clear();
    clear_keyboard();
#endif
#ifdef BACKLIGHT_ENABLE
    backlight_init();
#endif
}

#ifndef NO_SUSPEND_POWER_DOWN
/* watchdog timeout */
ISR(WDT_vect)
{
    // compensate timer for sleep
    switch (wdt_timeout) {
        case WDTO_15MS:
            // timer_count += 15 + 2;  // WDTO_15MS + 2(from observation)
            timer_count += 15 + 4;  // WDTO_15MS + 4 is more accurate on Teensy 2.0 (deneb's observation)
            break;
        default:
            ;
    }
}
#endif
