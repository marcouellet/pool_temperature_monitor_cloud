#include "data_transfer_cc1101.h"

DataTransferCC1101::DataTransferCC1101(CFREQ freq, uint8_t channr, uint8_t addr, uint8_t powerLevel) {
    while (!cc1101.begin(CFREQ_922, channr, addr));   
    cc1101.setOutputPowerLeveldBm(powerLevel); 
}

void DataTransferCC1101::setupForReceive() {
    cc1101.setRxAlways();
    cc1101.setRxState(); 
}

void DataTransferCC1101::sleep() {
    cc1101.setPowerDownState();
}

bool DataTransferCC1101::isDataAvailable() {
    return cc1101.dataAvailable();
}

DataTransferMessage* DataTransferCC1101::getData() {
    if ( isDataAvailable() )
    {       
         return  (DataTransferMessage*) cc1101.getChars(); 
    }
    else {
        return NULL;
    }
}

void DataTransferCC1101::sendData(const DataTransferMessage data, uint8_t destAddr) {
  cc1101.sendChars((const char *)&data, destAddr); 
}