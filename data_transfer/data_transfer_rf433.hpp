#ifndef DATA_TRANSFER_RF433_HPP
#define DATA_TRANSFER_RF433_HPP

#include <RH_E32.h>
#include "SoftwareSerial.h"

SoftwareSerial mySerial(7, 6);
RH_E32  driver(&mySerial, 4, 5, 8);
#include "data_transfer.hpp"

class DataTransferRF433 {
public:
  DataTransferRF433(SoftwareSerial* serial, byte m0_pin = 4, byte m1_pin = 5, byte aux_pin = 8);
  bool isDataAvailable();
  DataTransferMessage* getData();
  void sendData(const DataTransferMessage* message);
private:
  RH_E32 rhe32;
};

#endif
