/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

/**
 * Copy this file into your project directory and rename it as gfxconf.h
 * Edit your copy to turn on the GFX features you want to use.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use. One of these must be defined - preferably in your Makefile */
#define GFX_USE_OS_CHIBIOS  TRUE


/* GFX subsystems to turn on */
#define GFX_USE_GDISP           TRUE
#define GFX_USE_TDISP           FALSE
#define GFX_USE_GWIN            TRUE
#define GFX_USE_GEVENT          TRUE
#define GFX_USE_GTIMER          TRUE
#define GFX_USE_GQUEUE          TRUE
#define GFX_USE_GINPUT          TRUE
#define GFX_USE_GADC            FALSE
#define GFX_USE_GAUDIN          FALSE
#define GFX_USE_GAUDOUT         FALSE
#define GFX_USE_GMISC           FALSE

/* Features for the GDISP subsystem */

#define GDISP_STARTUP_LOGO_TIMEOUT  0
#define GDISP_STARTUP_COLOR         Black

#define GDISP_NEED_AUTOFLUSH        FALSE
#define GDISP_NEED_TIMERFLUSH       FALSE
#define GDISP_NEED_VALIDATION       TRUE
#define GDISP_NEED_CLIP             TRUE
#define GDISP_NEED_TEXT             TRUE
#define GDISP_NEED_CIRCLE           TRUE
#define GDISP_NEED_ELLIPSE          FALSE
#define GDISP_NEED_ARC              FALSE
#define GDISP_NEED_CONVEX_POLYGON   FALSE
#define GDISP_NEED_SCROLL           FALSE
#define GDISP_NEED_PIXELREAD        FALSE
#define GDISP_NEED_CONTROL          TRUE
#define GDISP_NEED_QUERY            FALSE
#define GDISP_NEED_IMAGE            TRUE
#define GDISP_NEED_MULTITHREAD      TRUE
#define GDISP_NEED_STREAMING        TRUE

/* GDISP - text features */
#define GDISP_NEED_ANTIALIAS        TRUE
#define GDISP_NEED_UTF8             FALSE
#define GDISP_NEED_TEXT_KERNING     FALSE

/* GDISP - fonts to include */
#define GDISP_INCLUDE_FONT_UI1                  FALSE
#define GDISP_INCLUDE_FONT_UI2                  FALSE
#define GDISP_INCLUDE_FONT_LARGENUMBERS         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS10         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS24         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32         FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12     FALSE
#define GDISP_INCLUDE_FONT_FIXED_10x20          FALSE
#define GDISP_INCLUDE_FONT_FIXED_7x14           FALSE
#define GDISP_INCLUDE_FONT_FIXED_5x8            FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12_AA      TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16_AA      TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS24_AA      FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32_AA      FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12_AA  FALSE
#define GDISP_INCLUDE_USER_FONTS                FALSE

/* GDISP image decoders */
#define GDISP_NEED_IMAGE_NATIVE     FALSE
#define GDISP_NEED_IMAGE_GIF        TRUE
#define GDISP_NEED_IMAGE_BMP        TRUE
#define GDISP_NEED_IMAGE_JPG        FALSE
#define GDISP_NEED_IMAGE_PNG        FALSE
#define GDISP_NEED_IMAGE_ACCOUNTING FALSE

/* Optional image support that can be turned off */
/*
    #define GDISP_NEED_IMAGE_BMP_1      TRUE
    #define GDISP_NEED_IMAGE_BMP_4      TRUE
    #define GDISP_NEED_IMAGE_BMP_4_RLE  TRUE
    #define GDISP_NEED_IMAGE_BMP_8      TRUE
    #define GDISP_NEED_IMAGE_BMP_8_RLE  TRUE
    #define GDISP_NEED_IMAGE_BMP_16     TRUE
    #define GDISP_NEED_IMAGE_BMP_24     TRUE
    #define GDISP_NEED_IMAGE_BMP_32     TRUE
*/

/* Features for the TDISP subsystem. */
#define TDISP_NEED_MULTITHREAD  FALSE

/* Features for the GWIN subsystem. */
#define GWIN_NEED_WINDOWMANAGER TRUE
#define GWIN_NEED_CONSOLE       TRUE
#define GWIN_NEED_GRAPH         FALSE
#define GWIN_NEED_WIDGET        TRUE
#define GWIN_NEED_LABEL         TRUE
#define GWIN_NEED_BUTTON        TRUE
#define GWIN_NEED_SLIDER        TRUE
#define GWIN_NEED_CHECKBOX      TRUE
#define GWIN_NEED_IMAGE         FALSE
#define GWIN_NEED_RADIO         TRUE
#define GWIN_NEED_LIST          TRUE
#define GWIN_NEED_PROGRESSBAR   TRUE

#define GWIN_CONSOLE_USE_FILLED_CHARS TRUE

/* Features for the GEVENT subsystem. */
#define GEVENT_ASSERT_NO_RESOURCE   FALSE

/* Features for the GQUEUE subsystem. */
#define GQUEUE_NEED_ASYNC       TRUE
#define GQUEUE_NEED_GSYNC       FALSE
#define GQUEUE_NEED_FSYNC       FALSE

/* Features for the GINPUT subsystem. */
#define GINPUT_NEED_MOUSE       TRUE
#define GINPUT_NEED_KEYBOARD    FALSE
#define GINPUT_NEED_TOGGLE      FALSE
#define GINPUT_NEED_DIAL        FALSE


#endif /* _GFXCONF_H */
