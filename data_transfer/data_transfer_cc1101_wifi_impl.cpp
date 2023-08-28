#include "data_transfer_cc1101_wifi_impl.hpp"
#include "trace.h"

DataTransferCC1101WifiImpl::DataTransferCC1101WifiImpl(CFREQ freq, byte channr, byte addr, byte powerLevel): 
    DataTransferCC1101(freq, channr, addr, powerLevel) {}

void DataTransferCC1101WifiImpl::configure() {
}