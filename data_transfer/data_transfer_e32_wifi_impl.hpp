#ifndef DATA_TRANSFER_E32_WIFI_IMPL_HPP
#define DATA_TRANSFER_E32_WIFI_IMPL_HPP

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include "data_transfer_e32.hpp"

class DataTransferE32WifiImpl : public DataTransferE32 {
public:
  DataTransferE32WifiImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin): DataTransferE32(serial, auxPin, m0Pin, m1Pin);
  virtual void configure(); 
};

#endif
