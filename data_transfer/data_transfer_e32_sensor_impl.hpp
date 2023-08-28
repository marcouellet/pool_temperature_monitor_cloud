#ifndef DATA_TRANSFER_E32_SENSOR_IMPL_HPP
#define DATA_TRANSFER_E32_SENSOR_IMPL_HPP

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include "data_transfer_e32.hpp"

class DataTransferE32SensorImpl : public DataTransferE32 {
public:
  DataTransferE32SensorImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin);
  virtual void configure(); 
};

#endif
