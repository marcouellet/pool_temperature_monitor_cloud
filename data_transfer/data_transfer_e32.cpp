#include "data_transfer_e32.hpp"
#include "trace.h"

DataTransferE32::DataTransferE32(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin) : e32ttl100(serial, auxPin, m0Pin, m1Pin) {
	e32ttl100.begin();
}

bool DataTransferE32::isDataAvailable() {
    return e32ttl100.available()  > 1;
}

Status DataTransferE32::setMode(MODE_TYPE mode) {
    return e32ttl100.setMode(mode);
}

DataTransferMessage* DataTransferE32::getData() {
    if ( isDataAvailable() )
    {       
       ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(DataTransferMessage));
        struct DataTransferMessage message = *(DataTransferMessage*) rsc.data;
        printMessageValues(message);

        if (strcmp(message.type, MESSAGE_TYPE_DATA)) {
            return &message;
        }

        rsc.close();
    }
    else {
        return NULL;
    }
}

ResponseStatus DataTransferE32::sendData(const DataTransferMessage data, byte ADDH, byte ADDL, byte CHAN) {
    Trace::traceln("Send message to E32 Wifi");
	ResponseStatus rs = e32ttl100.sendFixedMessage(ADDH, ADDL, CHAN, (void *)&data, sizeof(data));
	traceln(rs.getResponseDescription().c_str());
    return rs;
}

void printE32Parameters(struct Configuration configuration) {
    traceln("----------------------------------------");
 
    trace("HEAD BIN: ");  
    trace(configuration.HEAD, BIN);
    trace(" ");
    trace(configuration.HEAD, DEC);
    trace(" ");
    traceln(configuration.HEAD, HEX);
    traceln(" ");
    trace("AddH BIN: ");  
    traceln(configuration.ADDH, BIN);
    trace("AddL BIN: ");  
    traceln(configuration.ADDL, BIN);
    trace("Chan BIN: ");  
    trace(configuration.CHAN, DEC); 
    trace(" -> "); 
    traceln(configuration.getChannelDescription().c_str());
    traceln(" ");
    trace("SpeedParityBit BIN    : ");  
    trace(configuration.SPED.uartParity, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getUARTParityDescription().c_str());
    trace("SpeedUARTDataRate BIN : ");  
    trace(configuration.SPED.uartBaudRate, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getUARTBaudRate().c_str());
    trace("SpeedAirDataRate BIN  : ");  
    trace(configuration.SPED.airDataRate, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getAirDataRate().c_str());
    trace("OptionTrans BIN       : ");  
    trace(configuration.OPTION.fixedTransmission, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getFixedTransmissionDescription().c_str());
    trace("OptionPullup BIN      : ");  
    trace(configuration.OPTION.ioDriveMode, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getIODroveModeDescription().c_str());
    trace("OptionWakeup BIN      : ");  
    trace(configuration.OPTION.wirelessWakeupTime, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getWirelessWakeUPTimeDescription().c_str());
    trace("OptionFEC BIN         : ");  
    trace(configuration.OPTION.fec, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getFECDescription().c_str());
    trace("OptionPower BIN       : ");  
    trace(configuration.OPTION.transmissionPower, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getTransmissionPowerDescription().c_str());
 
    traceln("----------------------------------------");
 
}

void printE32ModuleInformation(struct ModuleInformation moduleInformation) {
    traceln("----------------------------------------");
    trace("HEAD BIN: ");  
    trace(moduleInformation.HEAD, BIN);
    trace(" ");
    trace(moduleInformation.HEAD, DEC);
    trace(" ");
    traceln(moduleInformation.HEAD, HEX);
 
    trace("Freq.: ");  
    traceln(moduleInformation.frequency, HEX);
    trace("Version  : ");  
    traceln(moduleInformation.version, HEX);
    trace("Features : ");  
    traceln(moduleInformation.features, HEX);
    traceln("----------------------------------------");
}