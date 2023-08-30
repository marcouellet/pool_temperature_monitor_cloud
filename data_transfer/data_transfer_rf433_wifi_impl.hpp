#ifndef DATA_TRANSFER_RF433_WIFI_IMPL_HPP
#define DATA_TRANSFER_RF433_WIFI_IMPL_HPP

#include "data_transfer_rf433.hpp"

class DataTransferRF433WifiImpl : public DataTransferRF433 {
public:
  DataTransferRF433WifiImpl(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false);
  virtual void configure(); 
};

#endif
