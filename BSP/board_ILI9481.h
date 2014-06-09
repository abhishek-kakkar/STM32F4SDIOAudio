/*
 * FSMC board file
 */

/**
 * @file    drivers/gdisp/ILI9481/gdisp_lld_board_example_fsmc.h
 * @brief   GDISP Graphics Driver subsystem low level driver source for
 * 			the ILI9481 and compatible HVGA display
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define GDISP_USE_FSMC
#define GDISP_USE_DMA

// For a multiple display configuration we would put all this in a structure and then
//  set g->board to that structure.
#define GDISP_REG              ((volatile uint16_t *) 0x60000000)[0] /* RS = 0 */
#define GDISP_RAM              ((volatile uint16_t *) 0x60020000)[0] /* RS = 1 */
#define GDISP_DMA_STREAM        STM32_DMA2_STREAM7
#define FSMC_BANK               0

static inline void init_board(GDisplay *g) {

    // As we are not using multiple displays we set g->board to NULL as we don't use it.
    g->board = 0;

    switch(g->controllerdisplay) {
    case 0:                                         // Set up for Display 0
        /**
         * Performs the following functions:
         *      1. initialise the io port used by the display
         *      2. initialise the reset pin (initial state not-in-reset)
         *      3. initialise the chip select pin (initial state not-active)
         *      4. initialise the backlight pin (initial state back-light off)
         */

        #if defined(STM32F1XX) || defined(STM32F3XX)
          /* FSMC setup for F1/F3 */
          rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

          #if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
              #error "DMA not implemented for F1/F3 Devices"
          #endif
        #elif defined(STM32F4XX) || defined(STM32F2XX)
            /* STM32F2-F4 FSMC init */
            rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

            #if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
                if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL)) chSysHalt();
                dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
                dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
            #endif
        #else
            #error "FSMC not implemented for this device"
        #endif

        /* set pins to FSMC mode */
        IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
                                (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

        IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
                            (1 << 13) | (1 << 14) | (1 << 15), 0};

        palSetBusMode(&busD, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_MID2);
        palSetBusMode(&busE, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_MID2);

        rccEnableTIM5(FALSE);

        TIM5->ARR = 255;
        TIM5->PSC = (STM32_TIMCLK1 / 8000000) - 1;
        TIM5->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
        TIM5->CCER = TIM_CCER_CC2E | TIM_CCER_CC2P;
        TIM5->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;

        TIM5->CCR2=100;

        palSetPadMode(GPIOA, 1, PAL_MODE_ALTERNATE(2));

        /* FSMC timing */
        //FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0;
        FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1 | FSMC_BTR1_BUSTURN_0 | FSMC_BTR1_BUSTURN_1;      /* FSMC timing */

        /* Bank1 NOR/SRAM control register configuration
         * This is actually not needed as already set by default after reset */
        FSMC_Bank1->BTCR[FSMC_BANK] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
        break;
    }
}

static inline void post_init_board(GDisplay *g) {
    (void) g;
}

static inline void setpin_reset(GDisplay *g, bool_t state) {
    (void) g;
    (void) state;
}

static inline void set_backlight(GDisplay *g, uint8_t percent) {
    (void) g;
    TIM5->CCR2 = percent * 100 / 255;
}

static inline void acquire_bus(GDisplay *g) {
    (void) g;
}

static inline void release_bus(GDisplay *g) {
    (void) g;
}

static inline void write_index(GDisplay *g, uint16_t index) {
    (void) g;
    GDISP_REG = index;
}

static inline void write_data(GDisplay *g, uint16_t data) {
    (void) g;
    GDISP_RAM = data;
}

static inline void setreadmode(GDisplay *g) {
    (void) g;
    FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1 | FSMC_BTR1_BUSTURN_0 | FSMC_BTR1_BUSTURN_1;      /* FSMC timing */
}

static inline void setwritemode(GDisplay *g) {
    (void) g;
    // FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0;;      /* FSMC timing */
    FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1 | FSMC_BTR1_BUSTURN_0 | FSMC_BTR1_BUSTURN_1;      /* FSMC timing */
}

static inline uint16_t read_data(GDisplay *g) {
    (void) g;
    uint16_t x, y;
    x = (GDISP_RAM >> 3) << 11;
    y = GDISP_RAM;
    y = ((y >> 10) << 5) | (y >> 2 & 0x1F);
    return (x | y);
}

#if defined(GDISP_USE_DMA) || defined(__DOXYGEN__)
    static inline void dma_with_noinc(GDisplay *g, color_t *buffer, int area) {
        (void) g;
        dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
        dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
        for (; area > 0; area -= 65535) {
            dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
            dmaStreamEnable(GDISP_DMA_STREAM);
            dmaWaitCompletion(GDISP_DMA_STREAM);
        }
    }

    static inline void dma_with_inc(GDisplay *g, color_t *buffer, int area) {
        (void) g;
        dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
        dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PINC | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
        for (; area > 0; area -= 65535) {
            dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
            dmaStreamEnable(GDISP_DMA_STREAM);
            dmaWaitCompletion(GDISP_DMA_STREAM);
        }
    }
#endif

#endif /* _GDISP_LLD_BOARD_H */
