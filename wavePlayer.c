/*
 * wavePlayer.c
 *
 * Copyright (C) 2012-14 Kumar Abhishek
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

#include "wavePlayer.h"
#include "ch.h"
#include "hal.h"

#include "gfx.h"
static WORKING_AREA(waThread3, 1024);

#define PLAYBACK_BUFFER_SIZE	8192

#define PLAYBACK_CIRCULAR

uint8_t buf[PLAYBACK_BUFFER_SIZE]={0};
#ifndef PLAYBACK_CIRCULAR
uint8_t buf2[PLAYBACK_BUFFER_SIZE]={0};
#endif

uint32_t waveSampleLength=0, bytesToPlay;

Thread* playerThread;

FIL f1;

extern stm32_dma_stream_t* i2sdma;

uint16_t vol=200;
uint8_t pause=0;
uint8_t flag = 0;

extern GHandle gConsole;
//BSEMAPHORE_DECL(dmaTransactionSemaphore, TRUE);

void dma_i2s_interrupt(void* dat, uint32_t flags) {
  // Semaphore signals TRUE for TC, FALSE for HT
  if (flags & STM32_DMA_ISR_TCIF)
    chEvtSignalI(playerThread, 1);

  if (flags & STM32_DMA_ISR_HTIF)
    chEvtSignalI(playerThread, 2);

}

static msg_t wavePlayerThread(void *arg) {
	(void)arg;

	UINT temp;
	UINT bufSwitch = 1;

	chRegSetThreadName("Player");

	codec_pwrCtl(1);
	codec_muteCtl(0);

	// Load the buffer with initial data
	wf_read(&f1, buf, sizeof(buf), &temp);
	bytesToPlay-=temp;

#ifdef PLAYBACK_CIRCULAR
	codec_audio_circular(buf, temp / sizeof(uint16_t));
#endif

	while(bytesToPlay)
	{
		eventmask_t evt = chEvtWaitOne(1 | 2);

#ifndef PLAYBACK_CIRCULAR
		if (bufSwitch)
		{
			codec_audio_send(buf, temp/2);
			wf_read(&f1, buf2, PLAYBACK_BUFFER_SIZE, &temp);
			bufSwitch=0;
		}
		else
		{
			codec_audio_send(buf2, temp/2);
			wf_read(&f1, buf, PLAYBACK_BUFFER_SIZE, &temp);
			bufSwitch=1;
		}
#else
        // Fill circular buffer
	    wf_read(&f1, evt == 1 ? buf + sizeof(buf) / 2 : buf, sizeof(buf) / 2, &temp);
#endif

		bytesToPlay-=temp;

		gwinPrintf(gConsole, "Bytes remain:%8ld\r", bytesToPlay);

		if (chThdShouldTerminate()) break;
	}

	codec_muteCtl(1);
	codec_pwrCtl(0);

	playerThread=NULL;

	wf_close(&f1);

	return 0;
}

void playWaveFile(void)
{
	uint8_t st;
	UINT btr;
	WAVFILE* wf;

	uint32_t temp, offset=44, cnt;

	if (playerThread) {
		return;
	}

	st = wf_open(&f1, "audio.wav", FA_READ);

	if (st) {
		gwinPrintf(gConsole, "File error = %d\r\n", st);
		return;
	}

	st = wf_read(&f1, buf, 1024, &btr);

	wf=(WAVFILE*)&buf[0];

	if (wf->riffSignature != WAVE_HEAD_RIFF || wf->fmtSignature != WAVE_HEAD_FMT || wf->format!=0x01) {
	  gwinPrintf(gConsole, "This is a not a wave file!\r\n");
		return;
	}

	gwinPrintf(gConsole, "%s, %d.%d kHz, %d-bit\r\n", wf->numChannels == 2 ? "Stereo" : "Mono", wf->sampleRate / 1000, (wf->sampleRate / 100)%10, wf->numBits);


	temp=READ_UINT32(&buf[36]);

	if (temp==WAVE_HEAD_DATA)
	{
		// Got 'data' chunk, ready to play
		waveSampleLength=READ_UINT32(&buf[40]);
		gwinPrintf(gConsole, "Playing file...\r\n");
		offset=44;
	}
	else if (temp==WAVE_META_LIST)
	{
		uint32_t msize;
		gwinPrintf(gConsole, "File has metadata:");
		msize=READ_UINT32(&buf[40]);
		offset=52+msize;
		gwinPrintf(gConsole, "%ld bytes.\r\nActual Data starts at offset:%ld\r\n", msize, offset);

		cnt=0;
		temp=READ_UINT32(&buf[44+cnt]);

		// Tag Reading Supported only on Audacity Output
		if (temp==WAVE_META_INFO)
		{
			cnt+=4;
			do
			{
				temp=READ_UINT32(&buf[44+cnt]);

				switch (temp)
				{
					case WAVE_META_INAM:
						cnt+=4;
						temp=READ_UINT32(&buf[44+cnt]);
						cnt+=4;
						gwinPrintf(gConsole, "Name: %s\r\n", &buf[44+cnt]);
						cnt+=temp;
						break;

					case WAVE_META_IART:
						cnt+=4;
						temp=READ_UINT32(&buf[44+cnt]);
						cnt+=4;
						gwinPrintf(gConsole, "Artist: %s\r\n", &buf[44+cnt]);
						cnt+=temp;
						break;

					case WAVE_META_ICMT:
						cnt+=4;
						temp=READ_UINT32(&buf[44+cnt]);
						cnt+=4;
						gwinPrintf(gConsole, "Composer: %s\r\n", &buf[44+cnt]);
						cnt+=temp;
						break;

					case WAVE_META_ICRD:
						cnt+=4;
						temp=READ_UINT32(&buf[44+cnt]);
						cnt+=4;
						gwinPrintf(gConsole, "Release Yr: %s\r\n", &buf[44+cnt]);
						cnt+=temp;
						break;

					default:
						cnt+=8;			// Token+Size (4 bytes each)
						break;
				}

			} while (cnt < msize);
			waveSampleLength=READ_UINT32(&buf[offset-4]);
			gwinPrintf(gConsole, "Ready To Play.\r\n");
		}
	}

	codec_i2s_init(wf->sampleRate, wf->numBits);

	cnt = waveSampleLength / (wf->sampleRate * wf->numBits * wf->numChannels / 8);
	gwinPrintf(gConsole, "Size:%ld bytes, Duration=%dm%02dsec\r\n", waveSampleLength, cnt / 60, cnt % 60);

	bytesToPlay=waveSampleLength;

	wf_lseek(&f1, offset);

	playerThread=chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO+3, wavePlayerThread, NULL);
}
