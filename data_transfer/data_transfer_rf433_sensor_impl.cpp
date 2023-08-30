#include "data_transfer_rf433_sensor_impl.hpp"

DataTransferRF433SensorImpl::DataTransferRF433SensorImpl(SoftwareSerial* serial, byte m0_pin, byte m1_pin, byte aux_pin): 
    DataTransferRF433(serial, m0_pin, m1_pin, aux_pin) {}

void DataTransferRF433SensorImpl::configure() {
}