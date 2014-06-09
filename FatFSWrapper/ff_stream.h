/*
 * ff_stream.h
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

#ifndef FF_STREAM_H_
#define FF_STREAM_H_

#include "ch.h"
#include "ff.h"


/**
 * @brief   FatFSFileStream specific methods.
 */
#define _ff_file_stream_methods                                             \
  _base_file_stream_methods													\
  uint32_t (*sync)(void *);													\
  uint32_t (*cache)(void *);

#define CACHE_SIZE			8192

/**
 * @brief   @p FatFSFileStream specific data.
 * @note    It is empty because @p BaseFileStream is only an interface
 *          without implementation.
 */
#define _ff_file_stream_data                                              \
  _base_file_stream_data 												  \
  FIL	filePtr;														  \
  uint32_t cachepos;													  \
  uint32_t filepos;														  \
  uint32_t cachesize;													  \
  uint8_t cache[CACHE_SIZE];

/**
 * @extends BaseSequentialStreamVMT
 *
 * @brief   @p BaseFileStream virtual methods table.
 */
struct FatFSFileStreamVMT {
  _ff_file_stream_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief   Base file stream class.
 * @details This class represents a generic file data stream.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct FatFSFileStreamVMT *vmt;
  _ff_file_stream_data
} FatFSFileStream;

#endif /* FF_STREAM_H_ */
