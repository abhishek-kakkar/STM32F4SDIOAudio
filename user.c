/*
 * user.c
 *
 * Copyright (C) 2014 Kumar Abhishek
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to the
 *  following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "ff_stream.h"
#include "fatFSWrapper.h"

#include "codec.h"
#include "wavePlayer.h"

extern FATFS SDC_FS;
extern uint8_t fbuff[1024];

coord_t swidth, sheight;

volatile int sig = 0;

#define PLAYBACK_BUFFER_SIZE    8192
extern uint8_t buf[PLAYBACK_BUFFER_SIZE ];

GHandle gConsole;

GWidgetInit pInit;

void userFn(void) {

    // Initialize the display
    gfxInit();

    gdispClear(Black);
    // Set the widget defaults
    gwinSetDefaultFont(gdispOpenFont("DejaVuSans12_aa"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);

    swidth = gdispGetWidth();
    sheight = gdispGetHeight();

    // Connect the mouse
//    #if GINPUT_NEED_MOUSE
//      gwinAttachMouse(0);
//    #endif

    pInit.g.show = TRUE;
    pInit.g.width = 200;
    pInit.g.height = 140;
    pInit.g.x = swidth - 5 - pInit.g.width;
    pInit.g.y = sheight - 5 - pInit.g.height;


    gConsole = gwinConsoleCreate(NULL, &(pInit.g));

    gwinSetColor(gConsole, Yellow);
    gwinSetBgColor(gConsole, Blue);

    gwinClear(gConsole);

    mountSDCard();
    chThdSleepMilliseconds(100);

    codec_test();

    int i = 0, f;

    while(1) {
      chThdSleepMilliseconds(1000);
    }
    return 0;
}

void codec_test(void) {
  uint8_t reg;

  reg = codec_readReg(0x01);
  gwinPrintf(gConsole, "CS43L22 Ver: %02X\r\n", reg);

  codec_hw_init();

  chThdSleepMilliseconds(100);

  playWaveFile();
}
