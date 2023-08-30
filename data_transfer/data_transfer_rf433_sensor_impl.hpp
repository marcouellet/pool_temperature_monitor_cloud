#ifndef DATA_TRANSFER_RF433_SENSOR_IMPL_HPP
#define DATA_TRANSFER_RF433_SENSOR_IMPL_HPP

#include "data_transfer_rf433.hpp"

class DataTransferRF433SensorImpl : public DataTransferRF433 {
public:
  DataTransferRF433SensorImpl(SoftwareSerial* serial, byte m0_pin, byte m1_pin, byte aux_pin);
  virtual void configure(); 
};

#endif
