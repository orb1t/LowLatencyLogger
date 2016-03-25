#ifndef sensors_h
#define sensors_h

#include<Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
unsigned long bmp_tmp_skip = 0;
unsigned long bmp_tmp_skip_div = 2;


double tTmp = 0;
struct bmp_data_t {
  double Temp,
         Prsr/*,
         Alt*/;   // 96 bits = 12 bytes
};

#define BMP_REC_SZ sizeof ( bmp_data_t )

struct bmp_data_b {
  int8_t buf[BMP_REC_SZ];
};

#define BMP_PRECISION 0

  

void bmp_read ( bmp_data_t* dt_ptr );

void bmp_setup( bmp_data_t* dt_ptr )
{
if (!bmp.begin(BMP_PRECISION)) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}


void bmp_read(bmp_data_t* dt_ptr){
  if ( bmp_tmp_skip++ % bmp_tmp_skip_div ) {
    dt_ptr->Temp = bmp.readTemperature();
  }
  //bmp_tmp_skip++;
  dt_ptr->Prsr = bmp.readPressure();
  //dt_ptr->Alt = bmp.readAltitude();
}


void bmp_dump(Print* pr, bmp_data_t* dt_ptr)
{
  pr->print(dt_ptr->Temp);
  pr->write(',');
  pr->print(dt_ptr->Prsr);
  pr->write(',');
  /*pr->print(dt_ptr->Alt);
  pr->write(",");*/
}



//=============================================================================
const int MPU_addr=0x68;  // I2C address of the MPU-6050

struct acc_data_t {   // 112 bits = 14 bytes
  int16_t Tmp, AcX,AcY,AcZ/*,
  GyX,GyY,GyZ*/;
};
#define ACC_REC_SZ  ( sizeof ( acc_data_t ) / sizeof ( int16_t ) ) + 1

struct acc_data_t1 {
  int16_t accl [ ACC_REC_SZ ];
};

void accel_setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}
void accel_read(acc_data_t* dt_ptr){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,8,true);  // request a total of 14 registers
  dt_ptr->AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  dt_ptr->AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  dt_ptr->AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  dt_ptr->Tmp = ( Wire.read() << 8 | Wire.read());  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  /*dt_ptr->GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  dt_ptr->GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  dt_ptr->GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  */
}

void accel_dump(Print* pr, acc_data_t1* dt_ptr){
  int i = 0;
  pr->print(dt_ptr->accl[i] / 340.00 + 36.53);
  
  for ( i = 1; i < ACC_REC_SZ - 1; i++ ) {
    pr->write(',');
    pr->print(dt_ptr->accl[i]);
  }
//  pr->print(dt_ptr->accl[i]);
}

#endif
