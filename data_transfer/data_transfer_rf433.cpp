#include "data_transfer_rf433.hpp"

DataTransferRF433::DataTransferRF433(SoftwareSerial* serial, byte m0_pin, byte m1_pin, byte aux_pin) : rhe32(serial, m0_pin, m1_pin, aux_pin){
    Serial.begin(9600);
    while (!Serial) ; // Wait for serial port to be available

    // Init the serial connection to the E32 module
    // which is assumned to be running at 9600baud.
    // If your E32 has been configured to another baud rate, change this:
    mySerial.begin(9600); 
    while (!mySerial) ;

    if (!driver.init())
    Serial.println("init failed");   
    // Defaults after initialising are:
    // 433MHz, 21dBm, 5kbps
    // You can change these as below
    //  if (!driver.setDataRate(RH_E32::DataRate1kbps))
    //    Serial.println("setDataRate failed"); 
    //  if (!driver.setPower(RH_E32::Power30dBm))
    //    Serial.println("setPower failed"); 
    //  if (!driver.setFrequency(434))
    //    Serial.println("setFrequency failed"); 
}

bool DataTransferRF433::isDataAvailable() {
    return rhe32.available();
}

DataTransferMessage* DataTransferRF433::getData() {
    if ( isDataAvailable() )
    {      
        DataTransferMessage* message = new DataTransferMessage;
        byte len = sizeof(DataTransferMessage);
        rhe32.recv((byte *)&message, &len);
        return message;
    }
    else {
        return NULL;
    }
}

void DataTransferRF433::sendData(const DataTransferMessage* message) {
    rhe32.send((byte *)message, sizeof(message));
    rhe32.waitPacketSent();
}