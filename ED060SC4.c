/*
 * 2013 Petteri Aimonen <jpa@gfx.mail.kapsi.fi>
 * This file is released to the public domain.
 */

/* Low-level E-ink panel driver routines for ED060SC4. */


#include <ED060SC4.h>
#include "contiki.h"
#include <stdio.h>
#include <i2c.h>
  #include <math.h>
#include <float.h>
#include "arial40all.h"
#include "septa.h"
#include "ietf.h"

/* =================================
 *      Default configuration
 * ================================= */

#ifndef GDISP_SCREEN_HEIGHT
#       define GDISP_SCREEN_HEIGHT 600
#endif

#ifndef GDISP_SCREEN_WIDTH
#       define GDISP_SCREEN_WIDTH 800
#endif

/* Number of pixels per byte */
#ifndef EINK_PPB
#       define EINK_PPB 4
#endif

/* Delay for generating clock pulses.
 * Unit is approximate clock cycles of the CPU (0 to 15).
 * This should be atleast 50 ns.
 */
#ifndef EINK_CLOCKDELAY
#       define EINK_CLOCKDELAY 1
#endif

/* Width of one framebuffer block.
 * Must be divisible by EINK_PPB and evenly divide GDISP_SCREEN_WIDTH. */
#ifndef EINK_BLOCKWIDTH
#       define EINK_BLOCKWIDTH 20
#endif

/* Height of one framebuffer block.
 * Must evenly divide GDISP_SCREEN_WIDTH. */
#ifndef EINK_BLOCKHEIGHT
#       define EINK_BLOCKHEIGHT 20
#endif

/* Number of block buffers to use for framebuffer emulation. */
#ifndef EINK_NUMBUFFERS
#       define EINK_NUMBUFFERS 40
#endif

/* Do a "blinking" clear, i.e. clear to opposite polarity first.
 * This reduces the image persistence. */
#ifndef EINK_BLINKCLEAR
#       define EINK_BLINKCLEAR 1
#endif

/* Number of passes to use when clearing the display */
#ifndef EINK_CLEARCOUNT
#       define EINK_CLEARCOUNT 10
#endif

/* Number of passes to use when writing to the display */
#ifndef EINK_WRITECOUNT
#       define EINK_WRITECOUNT 4
#endif

/* ====================================
 *      Lower level driver functions
 * ==================================== */


/** Delay between signal changes, to give time for IO pins to change state. */
inline static inline void clockdelay()
{
#if EINK_CLOCKDELAY & 1
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 2
    asm("nop");
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 4
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 8
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
#endif
}




/** Fast vertical clock pulse for gate driver, used during initializations */
inline void vclock_quick()
{
    gpio_set(GPIOB_EINK_CKV);
    asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_reset(GPIOB_EINK_CKV);
    asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
         nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
         nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
         nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");

}

/** Horizontal clock pulse for clocking data into source driver */
inline  void hclock()
{
    //asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
 asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_set(GPIOB_EINK_CL);
    asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_reset(GPIOB_EINK_CL);
}

/** Start a new vertical gate driver scan from top.
 * Note: Does not clear any previous bits in the shift register,
 *       so you should always scan through the whole display before
 *       starting a new scan.
 */
 void vscan_start()
{
    gpio_set(GPIOB_EINK_GMODE);
    vclock_quick();
    gpio_reset(GPIOB_EINK_SPV);
    vclock_quick();
    gpio_set(GPIOB_EINK_SPV);
    vclock_quick();
}

/** Waveform for strobing a row of data onto the display.
 * Attempts to minimize the leaking of color to other rows by having
 * a long idle period after a medium-length strobe period.
 */
static void vscan_write()
{
        gpio_set(GPIOB_EINK_CKV);
        gpio_set(GPIOB_EINK_OE);
        // 5us delay
         //clock_delay_usec(20);
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             ");
        gpio_reset(GPIOB_EINK_OE);
        gpio_reset(GPIOB_EINK_CKV);
        // 200us delay
        clock_delay_usec(183);
}

/** Waveform used when clearing the display. Strobes a row of data to the
 * screen, but does not mind some of it leaking to other rows.
 */
static void vscan_bulkwrite()
{
    gpio_set(GPIOB_EINK_CKV);
    clock_delay_usec(15);
    gpio_reset(GPIOB_EINK_CKV);
    clock_delay_usec(195);
}

/** Waveform for skipping a vertical row without writing anything.
 * Attempts to minimize the amount of change in any row.
 */
static void vscan_skip()
{
    gpio_set(GPIOB_EINK_CKV);
    asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_reset(GPIOB_EINK_CKV);
    clock_delay_usec(97);
}

/** Stop the vertical scan. The significance of this escapes me, but it seems
 * necessary or the next vertical scan may be corrupted.
 */
void vscan_stop()
{
    gpio_reset(GPIOB_EINK_GMODE);
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();
}

/** Start updating the source driver data (from left to right). */
static void hscan_start()
{
    /* Disable latching and output enable while we are modifying the row. */
    gpio_reset(GPIOB_EINK_LE);
    gpio_reset(GPIOB_EINK_OE);

    /* The start pulse should remain low for the duration of the row. */
    gpio_reset(GPIOB_EINK_SPH);
}

/** Write data to the horizontal row. */
static void hscan_write(const uint8_t *data, int count)
{
    while (count--)
    {
        /* Set the next byte on the data pins */
        setpins_data(*data++);

        /* Give a clock pulse to the shift register */
        hclock();
    }
}

/** Finish and transfer the row to the source drivers.
 * Does not set the output enable, so the drivers are not yet active. */
static void hscan_stop()
{
        gpio_set(GPIOB_EINK_SPH);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_set(GPIOB_EINK_CL);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_reset(GPIOB_EINK_CL);
        gpio_set(GPIOB_EINK_LE);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_reset(GPIOB_EINK_LE);
}

/** Turn on the power to the E-Ink panel, observing proper power sequencing. */
void power_on()
{
    unsigned i;

    /* First the digital power supply and signal levels. */
    gpio_set(GPIOB_EINK_VDD);
    gpio_reset(GPIOB_EINK_LE);
    gpio_reset(GPIOB_EINK_OE);
    gpio_reset(GPIOB_EINK_CL);
    gpio_set(GPIOB_EINK_SPH);
    setpins_data(0);
    gpio_reset(GPIOB_EINK_CKV);
    gpio_reset(GPIOB_EINK_GMODE);
    gpio_set(GPIOB_EINK_SPV);

    /* Min. 100 microsecond delay after digital supply */
    clock_delay_usec(100);

    // Turn on power supply
    i2c_enable();

    write_ti(0x03,0xA8);
    write_ti(0x05,0xFF);
    write_ti(0x06,0xFF);
    write_ti(0x01,0xBF);
 clock_delay_msec(100);
    /* Then negative voltages and min. 1000 microsecond delay. */
    //setpower_vneg(TRUE);
    //gfxSleepMicroseconds(1000);

    /* Finally the positive voltages. */
   // setpower_vpos(TRUE);

    /* Clear the vscan shift register */
    vscan_start();
    for (i = 0; i < GDISP_SCREEN_HEIGHT; i++) {
        vclock_quick();
    }
    vscan_stop();
}

/** Turn off the power, observing proper power sequencing. */
 void power_off()
{
    uint8_t temp;
    /* First the high voltages */
    // setpower_vpos(FALSE);
    // setpower_vneg(FALSE);
    temp = read_ti(0x01);
    temp |= 0x40;
    write_ti(0x01,temp);
    /* Wait for any capacitors to drain */
    clock_delay_msec(100);

    /* Then put all signals and digital supply to ground. */
    gpio_reset(GPIOB_EINK_LE);
    gpio_reset(GPIOB_EINK_OE);
    gpio_reset(GPIOB_EINK_CL);
    gpio_reset(GPIOB_EINK_SPH);

    setpins_data(0);
    gpio_reset(GPIOB_EINK_CKV);
    gpio_reset(GPIOB_EINK_GMODE);
    gpio_reset(GPIOB_EINK_GMODE);
    gpio_reset(GPIOB_EINK_VDD);
}



/* ====================================
 *      Framebuffer emulation layer
 * ==================================== */


#define PIXELMASK 3
#define PIXEL_WHITE 2
#define PIXEL_BLACK 1
#define BYTE_WHITE 0xAA
#define BYTE_BLACK 0x55


#if GDISP_SCREEN_HEIGHT % EINK_BLOCKHEIGHT != 0
#error GDISP_SCREEN_HEIGHT must be evenly divisible by EINK_BLOCKHEIGHT
#endif

#if GDISP_SCREEN_WIDTH % EINK_BLOCKWIDTH != 0
#error GDISP_SCREEN_WIDTH must be evenly divisible by EINK_BLOCKWIDTH
#endif

#if EINK_BLOCKWIDTH % EINK_PPB != 0
#error EINK_BLOCKWIDTH must be evenly divisible by EINK_PPB
#endif

#if EINK_NUMBUFFERS > 254
#error EINK_NUMBUFFERS must be at most 254.
#endif

#define BLOCKS_Y (GDISP_SCREEN_HEIGHT / EINK_BLOCKHEIGHT)
#define BLOCKS_X (GDISP_SCREEN_WIDTH / EINK_BLOCKWIDTH)
#define WIDTH_BYTES (EINK_BLOCKWIDTH / EINK_PPB)

/* Buffers that store the data for a small area of the display. */
typedef struct {
    uint8_t data[EINK_BLOCKHEIGHT][WIDTH_BYTES];
} block_t;

static uint8_t g_next_block; /* Index of the next free block buffer. */
static block_t g_blocks[EINK_NUMBUFFERS];

/* Map that stores the buffers associated to each area of the display.
 * Value of 0 means that the block is not allocated.
 * Other values are the index in g_blocks + 1.
 */
static uint8_t g_blockmap[BLOCKS_Y][BLOCKS_X];

/** Check if the row contains any allocated blocks. */
static int8_t blocks_on_row(unsigned by)
{
    unsigned bx;
    for (bx = 0; bx < BLOCKS_X; bx++)
    {
        if (g_blockmap[by][bx] != 0)
        {
            return 1;
        }
    }
    return 0;
}

/** Write out a block row. */
static void write_block_row(unsigned by)
{
    unsigned bx, dy, dx;
    for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
    {
        hscan_start();
        for (bx = 0; bx < BLOCKS_X; bx++)
        {
            if (g_blockmap[by][bx] == 0)
            {
                for (dx = 0; dx < WIDTH_BYTES; dx++)
                {
                    const uint8_t dummy = 0;
                    hscan_write(&dummy, 1);
                }
            }
            else
            {
                block_t *block = &g_blocks[g_blockmap[by][bx] - 1];
                hscan_write(&block->data[dy][0], WIDTH_BYTES);
            }
        }
        hscan_stop();

        vscan_write();
    }
}

/** Clear the block map, i.e. deallocate all blocks */
static void clear_block_map()
{
    unsigned bx, by;
    for (by = 0; by < BLOCKS_Y; by++)
    {
        for (bx = 0; bx < BLOCKS_X; bx++)
        {
            g_blockmap[by][bx] = 0;
        }
    }

    g_next_block = 0;
}

/** Flush all the buffered rows to display. */
static void flush_buffers()
{
    unsigned by, dy, i;

    for (i = 0; i < EINK_WRITECOUNT; i++)
    {
        vscan_start();

        for (by = 0; by < BLOCKS_Y; by++)
        {
            if (!blocks_on_row(by))
            {
                /* Skip the whole row of blocks. */
                for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
                {
                    vscan_skip();
                }
            }
            else
            {
                /* Write out the blocks. */
                write_block_row(by);
            }
        }

        vscan_stop();
    }

    clear_block_map();
}

/** Initialize a newly allocated block. */
static void zero_block(block_t *block)
{
    unsigned dx, dy;
    for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
    {
        for (dx = 0; dx < WIDTH_BYTES; dx++)
        {
            block->data[dy][dx] = 0;
        }
    }
}

/** Allocate a buffer
 * Automatically flushes if all buffers are full. */
static block_t *alloc_buffer(unsigned bx, unsigned by)
{
    block_t *result;
    if (g_blockmap[by][bx] == 0)
    {
        if (g_next_block >= EINK_NUMBUFFERS)
        {
            flush_buffers();
        }

        result = &g_blocks[g_next_block];
        g_blockmap[by][bx] = g_next_block + 1;
        g_next_block++;
        zero_block(result);
        return result;
    }
    else
    {
        result = &g_blocks[g_blockmap[by][bx] - 1];
        return result;
    }
}

/* ===============================
 *         Public functions
 * =============================== */

uint8_t gdisp_lld_init(void)
{
    init_board();

    /* Make sure that all the pins are in "off" state.
     * Having any pin high could cause voltage leaking to the
     * display, which in turn causes the image to leak slowly away.
     */
    power_off();

    clear_block_map();



    return 1;
}

void gdisp_lld_draw_pixel( uint16_t x, uint16_t y, uint8_t  color)
{
    block_t *block;
    uint8_t byte;
    unsigned bx, by, dx, dy;
    uint8_t bitpos;

    bx = x / EINK_BLOCKWIDTH;
    by = y / EINK_BLOCKHEIGHT;
    dx = x % EINK_BLOCKWIDTH;
    dy = y % EINK_BLOCKHEIGHT;

    if (bx < 0 || bx >= BLOCKS_X || by < 0 || by >= BLOCKS_Y)
        return;

    block = alloc_buffer(bx, by);

    bitpos = (6 - 2 * (dx % EINK_PPB));
    byte = block->data[dy][dx / EINK_PPB];
    byte &= ~(PIXELMASK << bitpos);
    if (color)
    {
        byte |= PIXEL_WHITE << bitpos;
    }
    else
    {
        byte |= PIXEL_BLACK << bitpos;
    }
    block->data[dy][dx / EINK_PPB] = byte;
}




/* ===============================
 *       Accelerated routines
 * =============================== */



 void subclear(uint8_t color)
{
    unsigned x, y;
    uint8_t byte;

    hscan_start();
    byte = color;
    for (x = 0; x < GDISP_SCREEN_WIDTH/4; x++)
    {
        hscan_write(&byte, 1);
    }
    hscan_stop();

    gpio_set(GPIOB_EINK_OE);
    vscan_start();
    for (y = 0; y < GDISP_SCREEN_HEIGHT; y++)
    {
        vscan_bulkwrite();
    }
    vscan_stop();
    gpio_reset(GPIOB_EINK_OE);
}

void gdisp_lld_clear(uint8_t color)
{
    unsigned i;
    clear_block_map();

    if (EINK_BLINKCLEAR)
    {
        subclear(!color);
        gfxSleepMilliseconds(50);
    }

    for (i = 0; i < EINK_CLEARCOUNT; i++)
    {
        subclear(color);
        gfxSleepMilliseconds(10);
    }

}

void set_one_color(){



    // vscan_start
    gpio_set(GPIOB_EINK_GMODE);
    vclock_quick();
    gpio_reset(GPIOB_EINK_SPV);
    vclock_quick();
    gpio_set(GPIOB_EINK_SPV);
    vclock_quick();


    int gate_driver, src_driver;

    for(gate_driver = 0; gate_driver < 600; gate_driver++){

        // hscan start
        gpio_reset(GPIOB_EINK_LE);
        gpio_reset(GPIOB_EINK_OE);
        gpio_reset(GPIOB_EINK_SPH);


        // hscan write
        for (src_driver = 0; src_driver < 200; src_driver++) {
            if(src_driver >75 && src_driver < 150 && gate_driver >300 && gate_driver < 400 )
                setpins_data(0x55);
            else
                setpins_data(0xAA);


            // 1us delay
            asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
            gpio_set(GPIOB_EINK_CL);
            // 1us delay
            asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
            gpio_reset(GPIOB_EINK_CL);
        }


        // hscan stop
        gpio_set(GPIOB_EINK_SPH);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_set(GPIOB_EINK_CL);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_reset(GPIOB_EINK_CL);
        gpio_set(GPIOB_EINK_LE);
        // 1us delay
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        gpio_reset(GPIOB_EINK_LE);


        // vscan write
        gpio_set(GPIOB_EINK_CKV);
        gpio_set(GPIOB_EINK_OE);
        // 5us delay
         //clock_delay_usec(20);
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
             ");
        gpio_reset(GPIOB_EINK_OE);
        gpio_reset(GPIOB_EINK_CKV);
        // 200us delay
        clock_delay_usec(183);
    }

    // vscan_stop
    gpio_reset(GPIOB_EINK_GMODE);
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();


}

void cycle_gate(){
        gpio_set(GPIOB_EINK_CKV);
               asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; ");
        gpio_reset(GPIOB_EINK_CKV);
}



void septa_logo(){

    uint8_t holder, temp, offset, scale = 1, screen_change;
    int8_t bitpos, scale_counter;

    uint8_t number_one = 0, num_one_offset_x = 10, num_one_offset_y =10;
    vscan_start();

    int gate_driver, src_driver, font_width = 64, font_height = 43, rowc =0, colc =0;

    for(gate_driver = 0; gate_driver < 600; gate_driver++){

        hscan_start();


        // hscan write
        for (src_driver = 0; src_driver < 200; src_driver++) {
            if(gate_driver >= num_one_offset_y && gate_driver <  num_one_offset_y + font_height*scale) {

                if(src_driver >= num_one_offset_x/4 && src_driver < num_one_offset_x/4 + (font_width*scale / 8)) {
                    screen_change = 1;
                    //printf("%d\n",colc);
                    for(scale_counter = scale; scale_counter >0; scale_counter-- ) {
                        for(bitpos = 3; bitpos >= 0; bitpos--) {

                            temp = 0x01 & (septabwBitmaps[colc] >>(bitpos + 4) ) ;

                            if(temp == 1) {
                                holder |= PIXEL_BLACK << bitpos*2;
                            } else {
                                holder |= PIXEL_WHITE << bitpos*2;
                            }
                        }
                        setpins_data(holder);
                        holder = 0;
                        hclock();
                    }

                    for(scale_counter = scale; scale_counter >0; scale_counter-- ) {
                        for(bitpos = 3; bitpos >= 0; bitpos--) {
                            temp = 0x01 & ( septabwBitmaps[colc]>>(bitpos) );

                            if(temp == 1) {
                                holder |= PIXEL_BLACK << bitpos*2;
                            } else {
                                holder |= PIXEL_WHITE << bitpos*2;
                            }
                        }
                        setpins_data(holder);
                        holder = 0;
                        hclock();
                    }

                    rowc++;
                    if(rowc%scale == 0 )
                        colc++;

                } else {
                    setpins_data(0x00);
                    hclock();
                }
            } else {
                setpins_data(0x00);
                hclock();
            }


        }


        hscan_stop();


        if(screen_change == 0)
            vscan_skip();
        else {
            vscan_write();
            screen_change = 0;
        }
    }

    vscan_stop();
}





void set_character(uint8_t txt_char, int offset_x, int offset_y){


uint8_t holder, temp, char_width, screen_change;
int8_t bitpos;

char_width = (ariald[txt_char][0] + 8 - 1)/8;


    vscan_start();

    int gate_driver, src_driver, font_height = ariald[txt_char][1], colc =0;

    for(gate_driver = 0; gate_driver < 600; gate_driver++){

        hscan_start();
        // hscan write
        for (src_driver = 0; src_driver < 200; src_driver++) {

            if(gate_driver >= offset_y && gate_driver <  offset_y + font_height) {

                  if(src_driver >= offset_x/4 && src_driver < offset_x/4 + char_width) {

                    screen_change = 1;
                    for(bitpos = 3; bitpos >= 0; bitpos--) {

                        temp = 0x01 & (arial[colc+ariald[txt_char][2]] >>(bitpos + 4) ) ;

                        if(temp == 1) {
                            holder |= PIXEL_BLACK << bitpos*2;
                        } else {
                            holder |= PIXEL_WHITE << bitpos*2;
                        }
                    }
                    setpins_data(holder);
                    holder = 0;
                    hclock();
                    for(bitpos = 3; bitpos >= 0; bitpos--) {
                        temp = 0x01 & (arial[colc+ariald[txt_char][2]]>>(bitpos) );

                        if(temp == 1) {
                            holder |= PIXEL_BLACK << bitpos*2;
                        } else {
                            holder |= PIXEL_WHITE << bitpos*2;
                        }
                    }
                    setpins_data(holder);
                    holder = 0;
                    hclock();

                    colc++;

                } else {
                    setpins_data(0x00);
                    hclock();
                }
            } else {
                setpins_data(0x00);
                hclock();
            }


        }


        hscan_stop();



        if(screen_change == 0)
            vscan_skip();
        else {
            vscan_write();
            screen_change = 0;
        }
    }

    vscan_stop();

}




void ietf_logo(){

    uint8_t holder, temp, offset, scale = 1, screen_change;
    int8_t bitpos, scale_counter;

    uint8_t number_one = 0;
    vscan_start();

    int gate_driver, src_driver, font_width = 496, font_height = 269, rowc =0, colc =0,num_one_offset_x = 150, num_one_offset_y =250;

    for(gate_driver = 0; gate_driver < 600; gate_driver++){

        hscan_start();


        // hscan write
        for (src_driver = 0; src_driver < 200; src_driver++) {
            if(gate_driver >= num_one_offset_y && gate_driver <  num_one_offset_y + font_height*scale) {

                if(src_driver >= num_one_offset_x/4 && src_driver < num_one_offset_x/4 + (font_width*scale / 8)) {
                    screen_change = 1;
                    //printf("%d\n",colc);
                    for(scale_counter = scale; scale_counter >0; scale_counter-- ) {
                        for(bitpos = 3; bitpos >= 0; bitpos--) {

                            temp = 0x01 & (ietfBitmaps[colc] >>(bitpos + 4) ) ;

                            if(temp == 1) {
                                holder |= PIXEL_BLACK << bitpos*2;
                            } else {
                                holder |= PIXEL_WHITE << bitpos*2;
                            }
                        }
                        setpins_data(holder);
                        holder = 0;
                        hclock();
                    }

                    for(scale_counter = scale; scale_counter >0; scale_counter-- ) {
                        for(bitpos = 3; bitpos >= 0; bitpos--) {
                            temp = 0x01 & ( ietfBitmaps[colc]>>(bitpos) );

                            if(temp == 1) {
                                holder |= PIXEL_BLACK << bitpos*2;
                            } else {
                                holder |= PIXEL_WHITE << bitpos*2;
                            }
                        }
                        setpins_data(holder);
                        holder = 0;
                        hclock();
                    }

                    rowc++;
                    if(rowc%scale == 0 )
                        colc++;

                } else {
                    setpins_data(0x00);
                    hclock();
                }
            } else {
                setpins_data(0x00);
                hclock();
            }


        }


        hscan_stop();


        if(screen_change == 0)
            vscan_skip();
        else {
            vscan_write();
            screen_change = 0;
        }
    }

    vscan_stop();
}
