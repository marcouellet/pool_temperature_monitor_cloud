#ifndef DATA_TRANSFER_CC1101_H
#define DATA_TRANSFER_CC1101_H

#include "cc1101.h"
#include "data_transfer.h"

class DataTransferCC1101 {
public:
  DataTransferCC1101(CFREQ freq, uint8_t channr, uint8_t addr, uint8_t powerLevel);
  void setupForReceive();
  bool isDataAvailable();
  void sleep();
  DataTransferMessage* getData();
  void sendData(const DataTransferMessage data, uint8_t destAddr);
private:
  CC1101 cc1101;
};

#endif
