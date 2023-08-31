#ifndef DATA_TRANSFER_RF24_HPP
#define DATA_TRANSFER_RF24_HPP

#include "RF24.h"
#include "data_transfer.hpp"

class DataTransferRF24 {
public:
  DataTransferRF24(rf24_gpio_pin_t _cepin, rf24_gpio_pin_t _cspin, uint32_t _spi_speed = RF24_SPI_SPEED);
  bool isDataAvailable();
  DataTransferMessage* getData();
  void sendData(const DataTransferMessage data);
private:
  RF24 rf24;
};

#endif
