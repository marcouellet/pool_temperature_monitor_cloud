#include "data_transfer_e32_sensor_impl.h"
#include "../../logging/trace.h"

DataTransferE32SensorImpl::DataTransferE32SensorImpl(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin): 
        DataTransferE32(serial, auxPin, m0Pin, m1Pin) {}

void DataTransferE32SensorImpl::configure() {
	// After set configuration comment set M0 and M1 to low
	// and reboot if you directly set HIGH M0 and M1 to program
  ResponseStructContainer c = e32ttl100.getConfiguration();
  // It's important get configuration pointer before all other operation
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDL = 0x01;
  configuration.ADDH = 0x00;
  configuration.CHAN = 0x02;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

  traceln(c.status.getResponseDescription().c_str());
  traceln(c.status.code);

  printE32Parameters(configuration);

  ResponseStructContainer cMi;
  cMi = e32ttl100.getModuleInformation();
  // It's important get information pointer before all other operation
  ModuleInformation mi = *(ModuleInformation*)cMi.data;

  traceln(cMi.status.getResponseDescription().c_str());
  traceln(cMi.status.code);

  printE32ModuleInformation(mi);
  c.close();}