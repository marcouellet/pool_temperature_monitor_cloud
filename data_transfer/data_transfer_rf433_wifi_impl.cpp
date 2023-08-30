#include "data_transfer_rf433_wifi_impl.hpp"

DataTransferRF433WifiImpl::DataTransferRF433WifiImpl(SoftwareSerial* serial, byte m0_pin, byte m1_pin, byte aux_pin): 
    DataTransferRF433(serial, m0_pin, m1_pin, aux_pin) {}

void DataTransferRF433WifiImpl::configure() {
}