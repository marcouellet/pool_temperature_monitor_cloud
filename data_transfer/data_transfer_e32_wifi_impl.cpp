#include "data_transfer_e32_wifi_impl.h"
#include "trace.h"

DataTransferE32WifiImpl::DataTransferE32WifiImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin): 
        DataTransferE32(serial, auxPin, m0Pin, m1Pin) {}

void DataTransferE32WifiImpl::configure() {
	// After set configuration comment set M0 and M1 to low
	// and reboot if you directly set HIGH M0 and M1 to program
	ResponseStructContainer c = e32ttl100.getConfiguration();
	Configuration configuration = *(Configuration*) c.data;
	configuration.ADDL = 3;
	configuration.ADDH = 0;
	configuration.CHAN = 0x04;
	configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;

	e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

    traceln(c.status.getResponseDescription().c_str());
    traceln(c.status.code);

	printE32Parameters(configuration);
	c.close();

    traceln("");
    traceln("Start listening!");
    e32ttl100.setMode(MODE_2_POWER_SAVING);
}