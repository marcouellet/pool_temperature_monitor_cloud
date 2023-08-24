#include <Ticker.h>
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>

#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "*************"
#include <BlynkSimpleEsp8266.h>

/*
 * LoRa E32-TTL-100
 * Send fixed broadcast transmission message to a specified channel.
 * https://www.mischianti.org/2019/11/10/lora-e32-device-for-arduino-esp32-or-esp8266-fixed-transmission-part-4/
 *
 * E32-TTL-100----- Arduino UNO or ESP8266
 * M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
 * M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
 * TX         ----- RX PIN 2 (PullUP)
 * RX         ----- TX PIN 3 (PullUP & Voltage divider)
 * AUX        ----- Not connected (5 if you connect)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */

#define MESSAGE_TYPE_DATA "DATA"          /* Temperature and charge data */

SoftwareSerial mySerial(D2, D3); // ESP8266 RX <-- e32 TX, ESP8266 TX --> e32 RX
LoRa_E32 e32ttl100(&mySerial, D5, D7, D6);

const int maxbauds = 115200;
int charge = 0;
int temperature = 0;
int isLowVoltage = 0;
int timeToSleep = 0;
bool disableTraceDisplay = false;
String  receive_payload;

// Wifi client section

const char* ssid = "**********";
const char* password = "**********";
const char* host = "**********";
const uint16_t port = 17;

// Blynk client section 

#define BLYNK_AUTH_TOKEN "****************************"

char blynkAuth[] = BLYNK_AUTH_TOKEN;
const char* blynkSsid = "**********";
const char* blynkPassword = "**********";
const int blynkVpinTemperature = 10;
const int blynkVpinCharge = 11;

void trace(const char * str) {
  if (!disableTraceDisplay) {
    Serial.print(str);
  }
}

void trace(byte b, int base) {
  if (!disableTraceDisplay) {
    Serial.print(b, base);
  }
}

void traceln(const char * str) {
   if (!disableTraceDisplay) {
    Serial.println(str);
  } 
}

void traceln(byte b) {
   if (!disableTraceDisplay) {
    Serial.println(b);
  } 
}

void traceln(byte b, int base) {
  if (!disableTraceDisplay) {
    Serial.println(b, base);
  }
}

void setupWifiClient() {
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        trace(".");
    }

    traceln("");
    traceln("WiFi connected");
    traceln("IP address: ");
    traceln(WiFi.localIP().toString().c_str());
}

void setupBlynkClient() {
    Blynk.begin(blynkAuth, blynkSsid, blynkPassword);

    traceln("");
    traceln("Blynk connected");
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

void setupE32Service() {
	e32ttl100.begin();

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

void sendDataToBlynkServer() {
    Blynk.virtualWrite(blynkVpinTemperature, temperature); //virtual pin V10
    Blynk.virtualWrite(blynkVpinCharge, charge); //virtual pin V11
}

void sendDataToWifiServer() {
    char str[150];
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    HTTPClient http;

    trace("[HTTP] begin...\n");
    // configure traged server and url
    sprintf(str, "http://%s/data", host);
    http.begin(client, str);  // HTTP
    http.addHeader("Content-Type", "application/json");

    trace("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    sprintf(str, "{""temperature"": ""%s"", ""charge"": ""%s""}", temperature, charge);
    int httpCode = http.POST(str);

    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        sprintf(str, "[HTTP] POST... code: %d\n", httpCode);
        trace(str);
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            const String& payload = http.getString();
            traceln("received payload:\n<<");
            traceln(payload.c_str());
            traceln(">>");
        }
        } else {
            sprintf(str, "[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
            trace(str);
        }

    http.end();
}

void setup()
{
    char str[150];
    Serial.begin(maxbauds);   
    delay(500);

    traceln("Setup started");

    sprintf(str, "Baud rate = %d seconds", maxbauds);
    traceln(str);

    //setupWifiClient();
    setupBlynkClient();
    setupE32Service();
}

struct Message {
    char type[5];
    int timeToSleep;
    int temperature;
    int charge;
    int isLowVoltage;
};

void printMessageValues(Message message) {
    char str[150];
    sprintf(str, "Message values: type=%s, sleep delay=%d seconds, water temperature=%d, charge=%d, alarm low voltage=%d", 
            message.type, message.timeToSleep, message.temperature, message.charge, message.isLowVoltage);
    traceln(str);
}

void loop()
{
    if (e32ttl100.available()  > 1){

        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(Message));
        struct Message message = *(Message*) rsc.data;
        printMessageValues(message);

        if (strcmp(message.type, MESSAGE_TYPE_DATA)) {
            temperature = message.temperature;
            charge = message.charge;
            isLowVoltage = message.isLowVoltage;
            timeToSleep = message.timeToSleep;
            //sendDataToWifiServer();
            sendDataToBlynkServer();
        }

    //		free(rsc.data);
        rsc.close();
    }
}

