#include "data_transfer_cc1101_sensor_impl.hpp"
#include "trace.h"

DataTransferCC1101SensorImpl::DataTransferCC1101SensorImpl(CFREQ freq, byte channr, byte addr, byte powerLevel): 
    DataTransferCC1101(freq, channr, addr, powerLevel) {}

void DataTransferCC1101SensorImpl::configure() {
}