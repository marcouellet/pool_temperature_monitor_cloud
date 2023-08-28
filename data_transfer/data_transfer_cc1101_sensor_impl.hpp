#ifndef DATA_TRANSFER_CC1101_SENSOR_IMPL_HPP
#define DATA_TRANSFER_CC1101_SENSOR_IMPL_HPP

#include "data_transfer_cc1101.hpp"

class DataTransferCC1101SensorImpl : public DataTransferCC1101 {
public:
  DataTransferCC1101SensorImpl(CFREQ freq, byte channr, byte addr, byte powerLevel);
  virtual void configure(); 
};

#endif
