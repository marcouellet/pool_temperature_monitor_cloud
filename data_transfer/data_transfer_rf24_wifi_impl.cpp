#include "data_transfer_rf24_wifi_impl.hpp"
#include "trace.h"

DataTransferRF24WifiImpl::DataTransferRF24WifiImpl(rf24_gpio_pin_t cepin, rf24_gpio_pin_t cspin, uint32_t spi_speed): 
    DataTransferRF24(cepin, cspin, spi_speed) {}

void DataTransferRF24WifiImpl::configure() {
}