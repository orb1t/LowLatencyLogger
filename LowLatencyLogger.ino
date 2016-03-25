/**
 * This program logs data to a binary file.  Functions are included
 * to convert the binary file to a csv text file.
 *
 * Samples are logged at regular intervals.  The maximum logging rate
 * depends on the quality of your SD card and the time required to
 * read sensor data.  This example has been tested at 500 Hz with
 * good SD card on an Uno.  4000 HZ is possible on a Due.
 *
 * If your SD card has a long write latency, it may be necessary to use
 * slower sample rates.  Using a Mega Arduino helps overcome latency
 * problems since 13 512 byte buffers will be used.
 *
 * Data is written to the file using a SD multiple block write command.
 */

//------------------------------------------------------------------------------
// User data functions.  Modify these functions for your data items.
//#include "UserDataType.h"  // Edit this include file to change data_t.
//#include "stuff.h"
#include "logger.h"
#include "sensors.h"

data_b tmp_buf;

/*void acc_read_tst(){
  //acc_data_t dt;
 accel_read((acc_data_t*)&tmp_buf); 
}

void bmp_read_tst(){
  //acc_data_t dt;
 bmp_read((bmp_data_t*)&tmp_buf); 
}

void sens_test(){
  acquireData(&tmp_buf);
}

void sens_min_test(){


 
}*/
bool stop_flag = false;
unsigned long run_cnt = 0;
//------------------------------------------------------------------------------
void setup(void) {
  if (ERROR_LED_PIN >= 0) {
    pinMode(ERROR_LED_PIN, OUTPUT);
  }
  Serial.begin(57600);
  while (!Serial) {}

  Serial.print(F("FastBufPtrLogger v0.0.9\nFreeRam: "));
  Serial.println(FreeRam());

Serial.print(F("Pkt/buf size: "));
Serial.print(sizeof(data_tt));
Serial.print(" : ");
Serial.println(sizeof(data_b));
Serial.print(sizeof(double));
  
  Serial.print(F("Pkts/block: "));
  Serial.print(DATA_DIM);
  Serial.print(F(", blk sz: "));
  Serial.print(sizeof(block_t));

Serial.print(F(", 512buf_blk_cnt: "));
  Serial.println  (BUFFER_BLOCK_COUNT);
/*Serial.print(F(", blk sz: "));
  Serial.print(sizeof(block_t));
    Serial.print(F(", blk sz: "));
  Serial.print(sizeof(block_t));
  */
  if (sizeof(block_t) != 512) {
    error("Invalid block size");
  }


  
  // initialize file system.
  if (!sd.begin(SD_CS_PIN, SPI_FULL_SPEED)) {
    sd.initErrorPrint();
    fatalBlink();
  }

  
  accel_setup();
  bmp_setup((bmp_data_t*)&tmp_buf);

  /*tFnkPtr fnk = bmp_read_tst;
 Serial.print("BMP 10 mesuremennts in micros : ");
 for ( int i = 0; i < 10; i++ )
    Serial.println(measure_milly(fnk));

   tFnkPtr fnk1 = acc_read_tst;
 Serial.print("ACC 10 mesuremennts in micros  : ");
 for ( int i = 0; i < 10; i++ )
  Serial.println(measure_milly(fnk1));

tFnkPtr fnk2 = sens_test;
 Serial.print("ALL sensors 10 mesuremennts in micros  : ");
 for ( int i = 0; i < 10; i++ )
  Serial.println(measure_milly(fnk2));

   MsTimer2::set(1000, flash); // 1000 ms  = 1 s
 MsTimer2::start();
 while ( ! stop_flag ) {
  
    fnk();
    fnk1();
    run_cnt++;
    //bmp_dump();
 }

   stop_flag = false;
 MsTimer2::set(10000, flash); // 1000 ms  = 1 s
 MsTimer2::start();
 while ( ! stop_flag ) {
    fnk2();
    run_cnt++;
    //bmp_dump();
 }*/

}


/*void flash(void) { 
  MsTimer2::stop();
 Serial.print("mesh / sec / div: ");
  Serial.print(run_cnt); Serial.print(" : " );
  Serial.println(bmp_tmp_skip_div);
  run_cnt = 0;
  bmp_tmp_skip = 0;
  stop_flag = true;
  //bmp_tmp_skip_div+=2;
  }*/ 


//------------------------------------------------------------------------------
void loop(void) {
  // discard any input
  while (Serial.read() >= 0) {}
  Serial.println();
  Serial.println(F("type:"));
  Serial.println(F("c - convert file to csv"));
  Serial.println(F("d - dump data to Serial"));
  Serial.println(F("e - overrun error details"));
  Serial.println(F("r - record data"));


  while(!Serial.available()) {

      delay(100);
      if ( dump_to_terminal ) {
        acquireData(&tmp_buf);
        printData(&Serial, &tmp_buf);
        
      }
    
  }
  char c = tolower(Serial.read());

  // Discard extra Serial data.
  do {
    delay(10);
  } while (Serial.read() >= 0);

  if (ERROR_LED_PIN >= 0) {
    digitalWrite(ERROR_LED_PIN, LOW);
  }
  if (c == 'c') {
    dump_to_terminal = false;
    binaryToCsv();
  } else if (c == 'd') {
    if (!binFile.isOpen()) {
      Serial.println();
      Serial.println(F("No current binary file! Dumping to Terminal"));
      dump_to_terminal = true;
    } else dumpData();
  } else if (c == 'e') {
    dump_to_terminal = false;
    checkOverrun();
  } else if (c == 'r') {
    dump_to_terminal = false;
    logData();
  } else {
    dump_to_terminal = false;
    Serial.println(F("Invalid entry"));
  }
}
