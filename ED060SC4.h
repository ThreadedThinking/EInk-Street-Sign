/*
 * 2013 Petteri Aimonen <jpa@gfx.mail.kapsi.fi>
 * This file is released to the public domain.
 * 
 */

/* Board interface definitions for ED060SC4 PrimeView E-ink panel.
 *
 * This file corresponds to the connections shown in example_schematics.png,
 * and is designed to interface with ChibiOS/RT.
 * 
 * Please note that this file has never been tested in exactly this pin
 * configuration, because the actual boards I have are slightly different.
 */
#include "contiki.h"
#include <stdio.h> 
#include <i2c.h>


/*
 * IO pins assignments.
 */
#define GPIOB_EINK_VDD      KBI3
#define GPIOB_EINK_GMODE    KBI6
#define GPIOB_EINK_SPV      KBI7
#define GPIOB_EINK_CKV      SSIRX
#define GPIOB_EINK_CL       SSITX
#define GPIOB_EINK_LE       BTCK
#define GPIOB_EINK_OE       FSYN
#define GPIOB_EINK_SPH      MISO
#define GPIOB_EINK_D0       ADC0
#define GPIOB_EINK_D1       ADC1
#define GPIOB_EINK_D2       ADC2
#define GPIOB_EINK_D3       ADC3
#define GPIOB_EINK_D4       ADC4
#define GPIOB_EINK_D5       ADC5
#define GPIOB_EINK_D6       ADC6
#define GPIOB_EINK_D7       KBI2




/* Set up IO pins for the panel connection. */
static inline void init_board(void) {
    
    // Datapins
    GPIO->PAD_DIR_SET.ADC0 = 1;
    GPIO->FUNC_SEL.ADC0    = 3;
    GPIO->PAD_DIR_SET.ADC1 = 1;
    GPIO->FUNC_SEL.ADC1    = 3;
    GPIO->PAD_DIR_SET.ADC2 = 1;
    GPIO->FUNC_SEL.ADC2    = 3;
    GPIO->PAD_DIR_SET.ADC3 = 1;
    GPIO->FUNC_SEL.ADC3    = 3;
    GPIO->PAD_DIR_SET.ADC4 = 1;
    GPIO->FUNC_SEL.ADC4    = 3;
    GPIO->PAD_DIR_SET.ADC4 = 1;
    GPIO->FUNC_SEL.ADC4    = 3;
    GPIO->PAD_DIR_SET.ADC5 = 1;
    GPIO->FUNC_SEL.ADC5    = 3;
    GPIO->PAD_DIR_SET.ADC6 = 1;
    GPIO->FUNC_SEL.ADC6    = 3;
    GPIO->PAD_DIR_SET.KBI2 = 1;
    GPIO->FUNC_SEL.KBI2   = 3;

    // SPH
    GPIO->PAD_DIR_SET.MISO = 1;
    GPIO->FUNC_SEL.MISO    = 3;   

    // VDD
    GPIO->PAD_DIR_SET.KBI3 = 1;
    GPIO->FUNC_SEL.KBI3    = 3;

    // GMODE
    GPIO->PAD_DIR_SET.KBI6 = 1;
    GPIO->FUNC_SEL.KBI6   = 3;

    // SPV
    GPIO->PAD_DIR_SET.KBI7 = 1;
    GPIO->FUNC_SEL.KBI7   = 3;

    // CKV
    GPIO->PAD_DIR_SET.SSIRX = 1;
    GPIO->FUNC_SEL.SSIRX   = 3;

    // CL
    GPIO->PAD_DIR_SET.SSITX = 1;
    GPIO->FUNC_SEL.SSITX   = 3;

    // LE
    GPIO->PAD_DIR_SET.BTCK = 1;
    GPIO->FUNC_SEL.BTCK   = 3;

    // OE
    GPIO->PAD_DIR_SET.FSYN = 1;
    GPIO->FUNC_SEL.FSYN   = 3;

    // SPH
    GPIO->PAD_DIR_SET.MISO = 1;
    GPIO->FUNC_SEL.MISO   = 3;
}

/* Delay for display waveforms. Should be an accurate microsecond delay. */
static void eink_delay(int us)
{
    //halPolledDelay(US2RTT(us));
    while(us--) {
        clock_delay_usec(1);
    }
}
//void power_on();
/* Set the state of the D0-D7 (source driver Data) pins. */
static inline void setpins_data(uint8_t value) {
    if(value & 1 ? gpio_set(ADC0) : gpio_reset(ADC0));
    if(value>>1 & 1 ? gpio_set(ADC1) : gpio_reset(ADC1));
    if(value>>2 & 1 ? gpio_set(ADC2) : gpio_reset(ADC2));
    if(value>>3 & 1 ? gpio_set(ADC3) : gpio_reset(ADC3));
    if(value>>4 & 1 ? gpio_set(ADC4) : gpio_reset(ADC4));
    if(value>>5 & 1 ? gpio_set(ADC5) : gpio_reset(ADC5));
    if(value>>6 & 1 ? gpio_set(ADC6) : gpio_reset(ADC6));
    if(value>>7 & 1 ? gpio_set(KBI2) : gpio_reset(KBI2));
}
inline  void hclock();
inline void vclock_quick();
 void vscan_start();
void vscan_stop();
void set_one_color();
void cycle_gate();
void set_one_color2();


