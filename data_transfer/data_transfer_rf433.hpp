#ifndef DATA_TRANSFER_RF433_HPP
#define DATA_TRANSFER_RF433_HPP

#include <RH_ASK.h>
#include "data_transfer.hpp"
class DataTransferRF433 {
public:
  DataTransferRF433(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false);
  bool isDataAvailable();
  DataTransferMessage* getData();
  void sendData(const DataTransferMessage* message);
private:
  RH_ASK rhask;
};

#endif
