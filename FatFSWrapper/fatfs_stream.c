/*
 * fatfs_stream.c
 *
 * Cached BaseFileStream for FatFS
 *
 *  Created on: 26-Jun-2013
 *
 * Copyright (C) 2013-2014 Kumar Abhishek
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
 */

#include "fatfsWrapper.h"
#include "ff_stream.h"
#include "ffconf.h"

static WORKING_AREA(waCacheThread, 64);
static msg_t fReqThread(msg_t *p) {
  FatFSFileStream *ptr = p;
  int dmy, res;

  res = wf_read(&(ptr->filePtr), ptr->cache, CACHE_SIZE, &dmy);
  ptr->cachesize = dmy;

  return res;
}

static Thread *p;

static uint32_t _cache(void *instance) {
  FatFSFileStream *ptr = instance;
  fileoffset_t br;

  p = chThdCreateStatic(waCacheThread, sizeof(waCacheThread), NORMALPRIO - 1, fReqThread, ptr);
}

/* Stream write buffer method.*/
static size_t write(void *instance, const uint8_t *bp, size_t n) {
  FatFSFileStream *ptr = instance;
  size_t bw;

  wf_write(&(ptr->filePtr), bp, n, &bw);

  return bw;
}

/* Stream read buffer method.*/
static size_t read(void *instance, uint8_t *bp, size_t n) {
  FatFSFileStream *ptr = instance;
  size_t br = 0, btr = n;

//  wf_read(&(ptr->filePtr), bp, n, &br);
  //btr = n + ptr->filepos - ptr->cachepos;

  if (n + ptr->filepos - ptr->cachepos <= CACHE_SIZE) {

    chThdWait(p);
    memcpy(bp, ptr->cache + ptr->filepos - ptr->cachepos, n);
    br += n;
  } else {
    // First, read till end of the cache
    memcpy(bp, ptr->cache + ptr->filepos - ptr->cachepos, CACHE_SIZE - (ptr->filepos - ptr->cachepos));
    btr -= CACHE_SIZE - (ptr->filepos - ptr->cachepos);
    bp += CACHE_SIZE - (ptr->filepos - ptr->cachepos);
    br += CACHE_SIZE - (ptr->filepos - ptr->cachepos);

    ptr->cachepos += CACHE_SIZE;
    _cache(instance);

    // Access contiguous blocks
    while (btr >= CACHE_SIZE) {
      if (!chThdTerminated(p)) {
        chThdWait(p);
      }
      memcpy(bp, ptr->cache, CACHE_SIZE);

      btr -= CACHE_SIZE;
      bp += CACHE_SIZE;
      br += CACHE_SIZE;

      ptr->cachepos += CACHE_SIZE;
      _cache(instance);
    }

    // Now copy the remaining bytes
    if (!chThdTerminated(p)) {
          chThdWait(p);
        }
    memcpy(bp, ptr->cache, btr);
    br += btr;
  }

  ptr->filepos += br;

  return br;
}

/* Channel put method, blocking.*/
static msg_t put(void *instance, uint8_t b) {
  return write(instance, &b, 1) == 1 ? CH_SUCCESS : CH_FAILED;
}

/* Channel get method, blocking.*/
static msg_t get(void *instance) {
  return 0;
}


static uint32_t close(void *instance) {
  FatFSFileStream *ptr = instance;

  return wf_close(&(ptr->filePtr));
}

/* Get last error code method.*/
static int geterror(void *instance) {
  FatFSFileStream *ptr = instance;

  return wf_error(&(ptr->filePtr));
}

/* File get size method.*/
fileoffset_t getsize(void *instance) {
  FatFSFileStream *ptr = instance;

  return wf_size(&(ptr->filePtr));
}

/* File get current position method.*/
fileoffset_t getposition(void *instance) {
  FatFSFileStream *ptr = instance;

  return ptr->filepos;
}

/* File seek method.*/
uint32_t lseek(void *instance, fileoffset_t offset) {
  FatFSFileStream *ptr = instance;
  int rel = (offset - ptr->cachepos) - CACHE_SIZE;
  fileoffset_t newCachePos, res;


  if (rel >= 0 && rel < CACHE_SIZE) {

  } else {
	newCachePos = offset / CACHE_SIZE;
	newCachePos *= CACHE_SIZE;
	res = wf_lseek(&(ptr->filePtr), newCachePos);
	if (res == FR_OK) {
	  ptr->cachepos = newCachePos;
	  res = _cache(instance);
	  if (res != FR_OK) {
		// Bad error cleanup
		return res;
	  }
	} else
	  return res;
  }
//  res = f_lseek(&ptr->filePtr, offset);
  ptr->filepos = offset;

  return res;
}

uint32_t sync(void *instance) {
  FatFSFileStream *ptr = instance;

  return wf_sync(&(ptr->filePtr));
}

static void cache(void *instance) {
  _cache(instance);
  chThdWait(p);
}

static const struct FatFSFileStreamVMT fatvmt = {
  write, read, put, get, close, geterror, getsize, getposition, lseek, sync, cache
};

FRESULT ff_OpenStream(FatFSFileStream *f, const char *path, uint8_t mode) {
   f->vmt = &fatvmt;
   f->filepos = 0;
   f->cachepos = 0;

   return wf_open(&(f->filePtr), path, mode);
}
