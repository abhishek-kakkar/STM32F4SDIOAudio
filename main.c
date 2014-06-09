/*
 * main.c
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
#include "ff.h"

#if 1
/*
 * SPI2 configuration structure.
 * Speed 21MHz, CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin 12 on the port GPIOA.
 */
static const SPIConfig spi2cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOB,
  12,
  SPI_CR1_DFF
};

/* I2C interface #1 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_16_9,
};

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
    chThdSleepMilliseconds(500);
  }
}

static FATFS SDC_FS;

uint16_t fbuff[2048];
#endif

int mountSDCard(void) {
  DIR d;
  int i;

  /* Power on SD Card */
//  palSetGroupMode(GPIOD, PAL_PORT_BIT(2), 0, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
  palSetGroupMode(GPIOE, PAL_PORT_BIT(2), 0, PAL_MODE_OUTPUT_PUSHPULL);

  // Turn on SD Card, allow voltage to rise
  palClearPad(GPIOE, 2);
  chThdSleepMilliseconds(10);

  // Start the SD Card
  sdcStart(&SDCD1, NULL);

  // Init the SD card and mount file-system.
  if (sdcConnect(&SDCD1))
	return 1;

  wf_mount(0, &SDC_FS);

  // Cache the first few FAT tables.
  wf_opendir(&d, "");

  return 0;
}

extern void userFn(void);

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  gfxInit();

  gdispClear(Black);

  /*
   * Activates the serial driver 2 using the driver default configuration.
   * PA2(TX) and PA3(RX) are routed to USART2.
   */
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  chSequentialStreamWrite(&SD2, "Welcome to ChibiOS\r\n", 20);

  wf_init(NORMALPRIO - 2);

  i2cStart(&I2CD1, &i2cfg1);

  gdispSetOrientation(GDISP_ROTATE_90);

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO - 2, Thread1, NULL);

  userFn();

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
