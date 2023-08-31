#ifndef DATA_TRANSFER_RF24_SENSOR_IMPL_HPP
#define DATA_TRANSFER_RF24_SENSOR_IMPL_HPP

#include "data_transfer_rf24.hpp"

class DataTransferRF24SensorImpl : public DataTransferRF24 {
public:
  DataTransferRF24SensorImpl(rf24_gpio_pin_t _cepin, rf24_gpio_pin_t _cspin, uint32_t _spi_speed = RF24_SPI_SPEED);
  virtual void configure(); 
};

#endif
