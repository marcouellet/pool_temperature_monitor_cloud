#include "data_transfer_rf433_wifi_impl.hpp"

DataTransferRF433WifiImpl::DataTransferRF433WifiImpl(uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted): 
    DataTransferRF433(speed, rxPin, txPin, pttPin, pttInverted) {}

void DataTransferRF433WifiImpl::configure() {
}