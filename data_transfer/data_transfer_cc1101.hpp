#ifndef DATA_TRANSFER_CC1101_HPP
#define DATA_TRANSFER_CC1101_HPP

#include "cc1101.h"
#include "data_transfer.hpp"

class DataTransferCC1101 {
public:
  DataTransferCC1101(CFREQ freq, byte channr, byte addr, byte powerLevel);
  virtual void configure() = 0; 
  void setupForReceive();
  bool isDataAvailable();
  void sleep();
  DataTransferMessage* getData();
  void sendData(const DataTransferMessage data, byte destAddr);
private:
  CC1101 cc1101;
};

#endif
