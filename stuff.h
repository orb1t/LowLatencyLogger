#ifndef stuff_h
#define stuff_h

#include "UserDataType.h"
#include "logger.h"
#include "sensors.h"

#include "MsTimer2.h"
typedef void (* tFnkPtr) ();
/*unsigned long measure_milly ( tFnkPtr fnk_ptr ) {
  //time00 = micros();
  unsigned long time01 = millis();

  fnk_ptr();
  
  //time11 = millis();
  //time10 = micros();

  return ( millis() - time01 );
}*/

// Digital pin to indicate an error, set to -1 if not used.
// The led blinks for fatal errors. The led goes on solid for SD write
// overrun errors and logging continues.
const int8_t ERROR_LED_PIN = -1;

// Print a data record.
void printData(Print* pr, data_b* dt_ptr) {
  bmp_dump ( pr, (bmp_data_t*)(&dt_ptr->buf[0]));//(data_t*)(&dt_ptr->PrsSens));
  accel_dump ( pr, (acc_data_t1*)(&dt_ptr->buf[sizeof(bmp_data_t)]) );//(acc_data_t*)dt_ptr->AccSens );
  pr->write ( '\n' );
  /*bmp_data_t* bmp = (bmp_data_t*)(&dt_ptr->buf[0]);
  //data_t* data = (data_t*)(&dt_ptr->buf[0]);
  pr->print(bmp->Temp);
  pr->write(',');
  pr->print(data->Pressure);
  for (int i = 0; i < ACCL_DATA_DIM; i++) {
    pr->write(',');
    pr->print(data->accl[i]);
  }*/
}

// Print data header.
/*void printHeader(Print* pr) {
  pr->print(F("time"));
  for (int i = 0; i < ACCL_DATA_DIM; i++) {
    pr->print(F(",adc"));
    pr->print(i);
  }
  pr->println();
}*/


//------------------------------------------------------------------------------
//
void fatalBlink() {
  while (true) {
    if (ERROR_LED_PIN >= 0) {
      digitalWrite(ERROR_LED_PIN, HIGH);
      delay(200);
      digitalWrite(ERROR_LED_PIN, LOW);
      delay(200);
    }
  }
}

//==============================================================================
// Error messages stored in flash.
#define error(msg) errorFlash(F(msg))
//------------------------------------------------------------------------------
void errorFlash(const __FlashStringHelper* msg) {
  sd.errorPrint(msg);
  fatalBlink();
}
//==============================================================================
// Convert binary file to csv file.
void binaryToCsv() {
  dump_to_terminal = false;
  uint8_t lastPct = 0;
  block_t block;
  uint32_t t0 = millis();
  uint32_t syncCluster = 0;
  SdFile csvFile;
  char csvName[13];

  if (!binFile.isOpen()) {
    Serial.println();
    Serial.println(F("No current binary file"));
    return;
  }
  binFile.rewind();
  // Create a new csvFile.
  strcpy(csvName, binName);
  strcpy(&csvName[BASE_NAME_SIZE + 3], "csv");

  if (!csvFile.open(csvName, O_WRITE | O_CREAT | O_TRUNC)) {
    error("open csvFile failed");
  }
  Serial.println();
  Serial.print(F("Writing: "));
  Serial.print(csvName);
  Serial.println(F(" - type any character to stop"));
  //printHeader(&csvFile);
  uint32_t tPct = millis();
  while (!Serial.available() && binFile.read(&block, 512) == 512) {
    uint16_t i;
    if (block.count == 0) {
      break;
    }
    if (block.overrun) {
      csvFile.print(F("OVERRUN,"));
      csvFile.println(block.overrun);
    }
    for (i = 0; i < block.count; i++) {
      printData(&csvFile, (data_b*)&block.data[i]);
    }
    if (csvFile.curCluster() != syncCluster) {
      csvFile.sync();
      syncCluster = csvFile.curCluster();
    }
    if ((millis() - tPct) > 1000) {
      uint8_t pct = binFile.curPosition()/(binFile.fileSize()/100);
      if (pct != lastPct) {
        tPct = millis();
        lastPct = pct;
        Serial.print(pct, DEC);
        Serial.println('%');
      }
    }
    if (Serial.available()) {
      break;
    }
  }
  csvFile.close();
  Serial.print(F("Done: "));
  Serial.print(0.001*(millis() - t0));
  Serial.println(F(" Seconds"));
}
//------------------------------------------------------------------------------
// read data file and check for overruns
void checkOverrun() {
    dump_to_terminal = false;
  bool headerPrinted = false;
  block_t block;
  uint32_t bgnBlock, endBlock;
  uint32_t bn = 0;

  if (!binFile.isOpen()) {
    Serial.println();
    Serial.println(F("No current binary file"));
    return;
  }
  if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
    error("contiguousRange failed");
  }
  binFile.rewind();
  Serial.println();
  Serial.println(F("Checking overrun errors - type any character to stop"));
  while (binFile.read(&block, 512) == 512) {
    if (block.count == 0) {
      break;
    }
    if (block.overrun) {
      if (!headerPrinted) {
        Serial.println();
        Serial.println(F("Overruns:"));
        Serial.println(F("fileBlockNumber,sdBlockNumber,overrunCount"));
        headerPrinted = true;
      }
      Serial.print(bn);
      Serial.print(',');
      Serial.print(bgnBlock + bn);
      Serial.print(',');
      Serial.println(block.overrun);
    }
    bn++;
  }
  if (!headerPrinted) {
    Serial.println(F("No errors found"));
  } else {
    Serial.println(F("Done"));
  }
}
//------------------------------------------------------------------------------
// dump data file to Serial
void dumpData() {
  block_t block;
    dump_to_terminal = false;
  
  binFile.rewind();
  Serial.println();
  Serial.println(F("Type any character to stop"));
  delay(1000);
//  printHeader(&Serial);
  while (!Serial.available() && binFile.read(&block , 512) == 512) {
    if (block.count == 0) {
      break;
    }
    if (block.overrun) {
      Serial.print(F("OVERRUN,"));
      Serial.println(block.overrun);
    }
    for (uint16_t i = 0; i < block.count; i++) {
      printData(&Serial, (data_b*)&block.data[i]);
    }
  }
  Serial.println(F("Done"));
}


#endif
