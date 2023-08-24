#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <MAX17043.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

#define DHTPIN 17     
#define DHTTYPE    DHT11
#define I2C_SDA 21
#define I2C_SCL 15

 /*
 * LoRa E32-TTL-100
 * Send fixed broadcast transmission message to a specified channel.
 * https://www.mischianti.org/2019/11/10/lora-e32-device-for-arduino-esp32-or-esp8266-fixed-transmission-part-4/
 *
 * E32-TTL-100----- Arduino UNO or esp8266
 * M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
 * M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
 * TX         ----- RX PIN 2 (PullUP)
 * RX         ----- TX PIN 3 (PullUP & Voltage divider)
 * AUX        ----- Not connected (5 if you connect)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */

SoftwareSerial mySerial(D2, D3); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX
LoRa_E32 e32ttl100(&mySerial, D5, D7, D6);

DHT dht(DHTPIN, DHTTYPE);

Ticker notificationTimer;
Ticker delayBeforeSleepTimer;
Ticker delayBetweenNotificationsTimer;
uint8_t notificationTimerId = 0;
uint8_t delayBeforeSleepTimerId = 1;
uint8_t delayBetweenNotificationsTimerId = 2;
uint32_t Freq = 0;
const int maxbauds = 115200;
uint16_t prescaler = 80;                    // Between 0 and 65 535
int threshold = 1000000;                    // 64 bits value (limited to int size of 32bits)
bool isPowerGaugeAvailable = false;
bool isPowerGaugeActive = false;
bool deviceConnected = false;
bool delayBetweenNotificationsTimeOut = false;
bool notificationTimeOut = false;
bool deepSleepRequested = false;
bool deepSleepReady = false;
int notificationRepeatCount = 0;
float voltage = 0;
float minVoltage = 2.7;
float minSafeVoltage = minVoltage * 1.1;
int charge = 0;
int waterTemperature;
bool disableTraceDisplay = false;

MAX17043 powerGauge(40);

// N.B. All delays are in seconds
#define uS_TO_S_FACTOR 1000000ULL         /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 * 5             /* Time ESP32 will stay in deep sleep before awakening (in seconds) */
#define TIME_TO_NOTIFY  30                /* Time ESP32 stay awaken to send notifications */
#define TIME_TO_WAIT_BEFORE_SLEEP  5      /* Time ESP32 stay awaken before gooing to deep sleep after notification period */
#define DELAY_BETWEEN_NOTIFICATIONS 5     /* Wait time between each notification send during notification period */
#define DELAY_TO_DISPLAY_SCREEN 5         /* Time to keep display active */
#define NOTIFICATION_REPEAT_COUNT_MAX  2  /* Max notifications to send during notification period */
#define SETUP_SENSORS_DELAY  0.5          /* Delay to read DS18B20 data before initialisation */

void trace(const char * str) {
  if (!disableTraceDisplay) {
    Serial.print(str);
  }
}

void trace(const char * str, int base) {
  if (!disableTraceDisplay) {
    Serial.print(str, base);
  }
}

void traceln(const char * str) {
   if (!disableTraceDisplay) {
    Serial.println(str);
  } 
}

int toFarenheit(int celcius) {
  double c = celcius;
  double f = c * 9.0 / 5.0 + 32.0;
  return (int) f;
}

void printE32Parameters(struct Configuration configuration) {
    traceln("----------------------------------------");
 
    trace(F("HEAD BIN: "));  
    trace(configuration.HEAD, BIN);
    trace(" ");
    trace(configuration.HEAD, DEC);
    trace(" ");
    traceln(configuration.HEAD, HEX);
    traceln(F(" "));
    trace(F("AddH BIN: "));  
    traceln(configuration.ADDH, BIN);
    trace(F("AddL BIN: "));  
    traceln(configuration.ADDL, BIN);
    trace(F("Chan BIN: "));  
    trace(configuration.CHAN, DEC); 
    trace(" -> "); 
    traceln(configuration.getChannelDescription());
    traceln(F(" "));
    trace(F("SpeedParityBit BIN    : "));  
    trace(configuration.SPED.uartParity, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getUARTParityDescription());
    trace(F("SpeedUARTDataRate BIN : "));  
    trace(configuration.SPED.uartBaudRate, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getUARTBaudRate());
    trace(F("SpeedAirDataRate BIN  : "));  
    trace(configuration.SPED.airDataRate, BIN);
    trace(" -> "); 
    traceln(configuration.SPED.getAirDataRate());
 
    trace(F("OptionTrans BIN       : "));  
    trace(configuration.OPTION.fixedTransmission, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getFixedTransmissionDescription());
    trace(F("OptionPullup BIN      : "));  
    trace(configuration.OPTION.ioDriveMode, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getIODroveModeDescription());
    trace(F("OptionWakeup BIN      : "));  
    trace(configuration.OPTION.wirelessWakeupTime, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getWirelessWakeUPTimeDescription());
    trace(F("OptionFEC BIN         : "));  
    trace(configuration.OPTION.fec, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getFECDescription());
    trace(F("OptionPower BIN       : "));  
    trace(configuration.OPTION.transmissionPower, BIN);
    trace(" -> "); 
    traceln(configuration.OPTION.getTransmissionPowerDescription());
 
    traceln("----------------------------------------");
 
}
void printE32ModuleInformation(struct ModuleInformation moduleInformation) {
    traceln("----------------------------------------");
    trace(F("HEAD BIN: "));  
    trace(moduleInformation.HEAD, BIN);
    trace(" ");
    trace(moduleInformation.HEAD, DEC);
    trace(" ");
    traceln(moduleInformation.HEAD, HEX);
 
    trace(F("Freq.: "));  
    traceln(moduleInformation.frequency, HEX);
    trace(F("Version  : "));  
    traceln(moduleInformation.version, HEX);
    trace(F("Features : "));  
    traceln(moduleInformation.features, HEX);
    traceln("----------------------------------------");
}

void setupE32Service() {
	e32ttl100.begin();

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

  traceln(c.status.getResponseDescription());
  traceln(c.status.code);

  printE32Parameters(configuration);

  ResponseStructContainer cMi;
  cMi = e32ttl100.getModuleInformation();
  // It's important get information pointer before all other operation
  ModuleInformation mi = *(ModuleInformation*)cMi.data;

  traceln(cMi.status.getResponseDescription());
  traceln(cMi.status.code);

  printE32ModuleInformation(mi);
  c.close();
}

bool isLowVoltage() {
  return voltage < minSafeVoltage;
}

bool isPowerGaugeAlert() {
  if (isPowerGaugeAvailable) {
    return powerGauge.isAlerting();
  }
  return false;
}

void checkPowerGaugeAvailable() {
  byte error;
  char str[50];
  traceln("Checking for MAX17043 device address ...");
  Wire.beginTransmission(MAX17043_ADDRESS);
  error = Wire.endTransmission();
    if (error == 0 || error == 5) {
      sprintf(str, "MAX17043 device found at address 0x%02X", MAX17043_ADDRESS);
      traceln(str);
      isPowerGaugeAvailable = true;
    }
    else {
      sprintf(str, "Error %d while accessing MAX17043 device at address 0x%02X", error, MAX17043_ADDRESS);
      traceln(str);
    }
}

void setupPowerGauge() {
  Wire.begin (I2C_SDA, I2C_SCL);
  pinMode (I2C_SDA, INPUT_PULLUP);
  pinMode (I2C_SCL, INPUT_PULLUP);
  checkPowerGaugeAvailable();
  if (isPowerGaugeAvailable) {
    char str[50];
    powerGauge.reset();
    powerGauge.quickStart();
    delay(100);
    sprintf(str, "MAX17043 version %d", powerGauge.getVersion());
    traceln(str);
    sprintf(str, "MAX17043 SOC %d %", powerGauge.getBatteryVoltage());
    traceln(str);
    sprintf(str, "MAX17043 voltage %d", powerGauge.getBatteryVoltage());
    traceln(str);
    // Sets the Alert Threshold to 10% of full capacity
    powerGauge.setAlertThreshold(10);
    sprintf(str, "MAX17043 Power Alert Threshold is set to %d %", powerGauge.getAlertThreshold());
    traceln(str);
    isPowerGaugeActive = true;
  }  
}

void notificationTimeEnded() {
  notificationTimeOut = true;
  cancelNotificationTimer();
}

bool isNotificationTimerActive() {
  return notificationTimer.active();
}

void cancelNotificationTimer() {
  if (notificationTimer.active()) {
    notificationTimer.detach();
  }
}

void setupNotification() {
  setupNotificationTimer(TIME_TO_NOTIFY);
  notificationRepeatCount = 0;
}

void setupNotificationTimer(int seconds) {
  cancelNotificationTimer(); // make sure no timer active
  // Attach onTimer function to our timer.
  notificationTimer.attach(seconds, notificationTimeEnded);
  notificationTimeOut = false;
}

void delayBeforeSleepTimerEnded() {
  cancelDelayBeforeSleepTimer();
  deepSleepReady = true;
}

bool isDelayBeforeSleepTimerActive() {
  return delayBeforeSleepTimer.active();
}

void cancelDelayBeforeSleepTimer() {
  if (delayBeforeSleepTimer.active()) {
      delayBeforeSleepTimer.detach();
  }
}

void setupDelayBeforeSleepTimer(int seconds) {
  cancelDelayBeforeSleepTimer(); // make sure no timer active
  delayBeforeSleepTimer.attach(seconds, delayBeforeSleepTimerEnded);
  deepSleepReady = false;
}

void delayBetweenNotificationsTimerEnded() {
  cancelDelayBetweenNotificationsTimer();
  delayBetweenNotificationsTimeOut = true;
}

bool isDelayBetweenNotificationsTimerActive() {
  return delayBetweenNotificationsTimer.active();
}

bool isDelayBetweenNotificationsTimerInactive() {
  return !delayBetweenNotificationsTimer.active();
}

void cancelDelayBetweenNotificationsTimer() {
  if (delayBetweenNotificationsTimer.active()) {
    delayBetweenNotificationsTimer.detach();
  }
}

void setupDelayBetweenNotificationsTimer(int seconds) {
  cancelDelayBetweenNotificationsTimer(); // make sure no timer active
  delayBetweenNotificationsTimer.attach(seconds, delayBetweenNotificationsTimerEnded);
  delayBetweenNotificationsTimeOut = false;
}

void setupSensors() {
  dht.begin();
  delay(1000*SETUP_SENSORS_DELAY);
  setupPowerGauge();
}

void readTemperature() {
  waterTemperature = dht.readTemperature();
}

void readSensors() {
  readTemperature();
  if (isPowerGaugeActive) {
    charge = powerGauge.getBatteryPercentage();
    voltage = powerGauge.getBatteryPercentage();
  }
}

void deepSleep() {
    char str[50];
    deepSleepRequested = false;
    deepSleepReady = false;

    sprintf(str, "Going to deep sleep for %d seconds", TIME_TO_SLEEP);
    traceln(str);

    ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR, RF_DEFAULT);
}

void printSensorsValues() {
    char str[150];
    sprintf(str, "Notify values: sleep delay=%d seconds, water temperature=%d, charge=%d, alarm low voltage=%d", 
            TIME_TO_SLEEP, waterTemperature, charge, isLowVoltage());
    traceln(str);
}

void notifySensorsValues() {
    String str = "";
    str += TIME_TO_SLEEP;
    str += ",";
    str += waterTemperature;
    str += ",";
    str += charge;
    str += ",";
    str += isLowVoltage() ? "1" : "0";
    sendE32Data(str);
    printSensorsValues();
}

void sendE32Data(const char * data) {
	traceln("Send message to E32 Wifi");
	ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0x04, data);
	traceln(rs.getResponseDescription());
}

void setup() {
  char str[150];
  Serial.begin(maxbauds);   
  delay(500);

  traceln("Setup started");

  sprintf(str, "Baud rate = %d seconds", maxbauds);
  traceln(str);

  setupSensors();
  readSensors();
    
  setupNotification();
  //TODO setupE32Service(); 
}

void loop() {

  if (deviceConnected && !notificationTimeOut && notificationRepeatCount < NOTIFICATION_REPEAT_COUNT_MAX) { 
    if (delayBetweenNotificationsTimeOut) {
      notificationRepeatCount++;
      notifySensorsValues();
      traceln("Device notified");  
    }
    if (notificationRepeatCount == 0 && isDelayBetweenNotificationsTimerInactive()) {
      delayBetweenNotificationsTimeOut = true; // No delay for first notification   
    } else if (delayBetweenNotificationsTimeOut && NOTIFICATION_REPEAT_COUNT_MAX > 1) {
      setupDelayBetweenNotificationsTimer(DELAY_BETWEEN_NOTIFICATIONS);
    }
  }

  if (notificationTimeOut || notificationRepeatCount >= NOTIFICATION_REPEAT_COUNT_MAX) {

      cancelDelayBetweenNotificationsTimer();
      cancelNotificationTimer();

      if (!deepSleepRequested) {
        if (notificationTimeOut) {
          traceln("Device notification timeout");     
        } else {
          traceln("Device notification done");
        }
        setupDelayBeforeSleepTimer(TIME_TO_WAIT_BEFORE_SLEEP); 
        deepSleepRequested = true;
        traceln("Deep sleep requested");
      }
  }

  if (deepSleepReady) {
      deepSleep();
  }

  if (isPowerGaugeAlert())
  {
      traceln("Beware, Low Power!");
  }
}
