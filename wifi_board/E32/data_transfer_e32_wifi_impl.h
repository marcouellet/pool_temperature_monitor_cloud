#ifndef DATA_TRANSFER_E32_WIFI_IMPL_H
#define DATA_TRANSFER_E32_WIFI_IMPL_H

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include "../../data_transfer/data_transfer_e32.h"

class DataTransferE32WifiImpl : DataTransferE32 {
public:
  DataTransferE32WifiImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin): DataTransferE32(serial, auxPin, m0Pin, m1Pin);
  virtual void configure(); 
};

#endif
