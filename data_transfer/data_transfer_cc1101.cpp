#include "data_transfer_cc1101.hpp"

DataTransferCC1101::DataTransferCC1101(CFREQ freq, byte channr, byte addr, byte powerLevel) {
    while (!cc1101.begin(freq, channr, addr));   
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

void DataTransferCC1101::sendData(const DataTransferMessage data, byte destAddr) {
  cc1101.sendChars((const char *)&data, destAddr); 
}