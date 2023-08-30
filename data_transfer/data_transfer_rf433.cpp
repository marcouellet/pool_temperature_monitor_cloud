#include "data_transfer_rf433.hpp"
#include "trace.h"

DataTransferRF433::DataTransferRF433(uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted) : 
    rhask(speed, rxPin, txPin, pttPin, pttInverted){
    if (!rhask.init())
         traceln("RF433 init failed");
}

bool DataTransferRF433::isDataAvailable() {
    return rhask.available();
}

DataTransferMessage* DataTransferRF433::getData() {
    if ( isDataAvailable() )
    {      
        DataTransferMessage* message = new DataTransferMessage;
        byte len = sizeof(DataTransferMessage);
        rhask.recv((byte *)&message, &len);
        return message;
    }
    else {
        return NULL;
    }
}

void DataTransferRF433::sendData(const DataTransferMessage* message) {
    rhask.send((byte *)message, sizeof(message));
    rhask.waitPacketSent();
}