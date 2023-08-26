#ifndef DATA_TRANSFER_E32_SENSOR_IMPL_H
#define DATA_TRANSFER_E32_SENSOR_IMPL_H

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include "data_transfer_e32.h"

class DataTransferE32SensorImpl : public DataTransferE32 {
public:
  DataTransferE32SensorImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin): DataTransferE32(serial, auxPin, m0Pin, m1Pin);
  virtual void configure(); 
};

#endif
