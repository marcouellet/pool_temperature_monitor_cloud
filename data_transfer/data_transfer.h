#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#define MESSAGE_TYPE_DATA "DATA"          /* Temperature and charge data */

struct DataTransferMessage {
    char type[5];
    int timeToSleep;
    int temperature;
    int charge;
    int isLowVoltage;
};

extern void printMessageValues(DataTransferMessage message);

#endif
