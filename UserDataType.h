#ifndef UserDataType_h
#define UserDataType_h

//#include "stuff.h"

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>

#include "sensors.h"

bool dump_to_terminal = false;

SdFat sd;
SdBaseFile binFile;


//const uint8_t ACCL_DATA_DIM = 7;  // 7 items ( including '0' ), 16 bits each
struct data_tt {
  bmp_data_t bmpSens; // 96 bits = 12 bytes
  acc_data_t1 accSens;  // 112 bits = 14 bytes
};
struct data_t {
  bmp_data_t* bmpSens;
  acc_data_t1* accSens;
};

const uint8_t PKT_DATA_DIM = sizeof(data_tt);//BMP_REC_SZ + ACC_REC_SZ;//( ACCL_DATA_DIM - 1 ) * 2 + 4 * 2;

struct data_b {
  int8_t buf[PKT_DATA_DIM];
};


//------------------------------------------------------------------------------
// File definitions.
//
// Maximum file size in blocks.
// The program creates a contiguous file with FILE_BLOCK_COUNT 512 byte blocks.
// This file is flash erased using special SD commands.  The file will be
// truncated if logging is stopped early.
const uint32_t FILE_BLOCK_COUNT = 256000;

// log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "data"
//------------------------------------------------------------------------------
// Buffer definitions.
//
// The logger will use SdFat's buffer plus BUFFER_BLOCK_COUNT additional
// buffers.
//
#ifndef RAMEND
// Assume ARM. Use total of nine 512 byte buffers.
const uint8_t BUFFER_BLOCK_COUNT = 8;
//
#elif RAMEND < 0X8FF
#error Too little SRAM
//
#elif RAMEND < 0X10FF
// Use total of two 512 byte buffers.
const uint8_t BUFFER_BLOCK_COUNT = 1;
//
#elif RAMEND < 0X20FF
// Use total of five 512 byte buffers.
const uint8_t BUFFER_BLOCK_COUNT = 4;
//
#else  // RAMEND
// Use total of 13 512 byte buffers.
const uint8_t BUFFER_BLOCK_COUNT = 12;
#endif  // RAMEND
//==============================================================================
// End of configuration constants.
//==============================================================================
// Temporary log file.  Will be deleted if a reset or power failure occurs.
#define TMP_FILE_NAME "tmp_log.bin"

// Size of file base name.  Must not be larger than six.
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;



char binName[13] = FILE_BASE_NAME "00.bin";

// Number of data records in a block.
const uint16_t DATA_DIM = (512 - 4)/sizeof(data_tt);

//Compute fill so block size is 512 bytes.  FILL_DIM may be zero.
const uint16_t FILL_DIM = 512 - 4 - DATA_DIM*sizeof(data_tt);

struct block_t {
  uint16_t count;
  uint16_t overrun;
  data_tt data[DATA_DIM];
  uint8_t fill[FILL_DIM];
};



#endif  // UserDataType_h
