#include "data_transfer_rf433_sensor_impl.hpp"

DataTransferRF433SensorImpl::DataTransferRF433SensorImpl(uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted): 
    DataTransferRF433(speed, rxPin, txPin, pttPin, pttInverted) {}

void DataTransferRF433SensorImpl::configure() {
}