#include "data_transfer.h"
#include "trace.h"

void printMessageValues(DataTransferMessage message) {
    char str[150];
    sprintf(str, "Message values: type=%s, sleep delay=%d seconds, water temperature=%d, charge=%d, alarm low voltage=%d", 
            message.type, message.timeToSleep, message.temperature, message.charge, message.isLowVoltage);
    traceln(str);
}