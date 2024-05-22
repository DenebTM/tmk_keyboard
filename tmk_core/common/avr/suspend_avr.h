#ifndef SUSPEND_AVR_H
#define SUSPEND_AVR_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


void wdt_intr_enable(uint8_t timeout) {
    // disable interrupts and reset watchdog
    cli();
    wdt_reset();

    // clear WDRF in MCUSR
    MCUSR &= ~(1<<WDRF);

    // start timed sequence to change WDE and prescaler bits
    WDTCSR |= (1 << WDCE);
    {
        // set prescaler bits
        WDTCSR = ((timeout & 0b111) << WDP0) | ((timeout & 0b1000) << WDP3);

        // disable watchdog reset mode
        WDTCSR &= ~(1 << WDE);
    }

    // enable watchdog interrupt mode
    WDTCSR |= (1 << WDIE);

    // re-enable interrupts
    sei();
}

#endif
