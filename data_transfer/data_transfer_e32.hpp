#ifndef DATA_TRANSFER_E32_HPP
#define DATA_TRANSFER_E32_HPP

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include "data_transfer.hpp"

class DataTransferE32 {
public:
  DataTransferE32(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin);
  virtual void configure() = 0; 
  Status setMode(MODE_TYPE mode);
  bool isDataAvailable();
  DataTransferMessage* getData();
  ResponseStatus sendData(const DataTransferMessage data, byte ADDH, byte ADDL, byte CHAN);
protected:
  LoRa_E32 e32ttl100;
};

extern void printE32Parameters(struct Configuration configuration);
extern void printE32ModuleInformation(struct ModuleInformation moduleInformation);

#endif
