/*
 * wavePlayer.h
 *
 *  Created on: Jun 8, 2012
 *
 * Copyright (C) 2012-2014 Kumar Abhishek
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

#ifndef WAVEPLAYER_H_
#define WAVEPLAYER_H_

#include "ch.h"
#include "hal.h"

#include "ff.h"

#include "codec.h"

#include "chprintf.h"

#define READ_UINT32(ptr)		*((uint32_t*)ptr)
#define READ_UINT16(ptr)		*((uint16_t*)ptr)

#define WAVE_HEAD_RIFF			0x46464952		// 'FIRR' = RIFF in Little-Endian format
#define WAVE_HEAD_WAVE			0x45564157		// 'WAVE' in Little-Endian
#define WAVE_HEAD_DATA			0x61746164		// 'data' in Little-Endian
#define WAVE_HEAD_FMT			0x20746D66

#define WAVE_META_LIST			0x5453494C
#define WAVE_META_INFO			0x4F464E49
#define WAVE_META_INAM			0x4D414E49
#define WAVE_META_IART			0x54524149
#define WAVE_META_ICMT			0x544D4349
#define WAVE_META_ICRD			0x44524349

typedef struct _wavfile {
	uint32_t riffSignature;
	uint32_t fileSize;
	uint32_t waveSignature;
	uint32_t fmtSignature;
	uint32_t subChunk1Size;
	uint16_t format;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t numBits;
} WAVFILE;

extern Thread* playerThread;

#endif /* WAVEPLAYER_H_ */
