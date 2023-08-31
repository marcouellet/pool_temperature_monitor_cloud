#include "data_transfer_rf24.hpp"
#include "trace.h"

DataTransferRF24::DataTransferRF24(rf24_gpio_pin_t cepin, rf24_gpio_pin_t cspin, uint32_t spi_speed) : 
    rf24(cepin, cspin, spi_speed) {
    if (!rf24.begin()){
         traceln("RF24 init failed");
    } 
}

bool DataTransferRF24::isDataAvailable() {
    return rf24.available();
}

DataTransferMessage* DataTransferRF24::getData() {
    if ( isDataAvailable() )
    {       
        DataTransferMessage* message = new DataTransferMessage;
        byte len = sizeof(DataTransferMessage);
        rf24.read((char *)&message, len);
        return message;
    }
    else {
        return NULL;
    }
}

void DataTransferRF24::sendData(const DataTransferMessage data) {
  rf24.write((const char *)&data, sizeof(DataTransferMessage)); 
}