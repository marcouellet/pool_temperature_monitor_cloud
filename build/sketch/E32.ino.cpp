#include <Arduino.h>
#line 1 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <MAX17043.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <EByte_LoRa_E32_library.h>

#define I2C_SDA 21
#define I2C_SCL 15

// GPIO where the DS18B20 is connected to
const int oneWireBus = 2;

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
int airTemperature;
bool disableTraceDisplay = false;

MAX17043 powerGauge(40);

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// N.B. All delays are in seconds
#define uS_TO_S_FACTOR 1000000ULL         /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 * 5             /* Time ESP32 will stay in deep sleep before awakening (in seconds) */
#define TIME_TO_NOTIFY  30                /* Time ESP32 stay awaken to send notifications */
#define TIME_TO_WAIT_BEFORE_SLEEP  5      /* Time ESP32 stay awaken before gooing to deep sleep after notification period */
#define DELAY_BETWEEN_NOTIFICATIONS 5     /* Wait time between each notification send during notification period */
#define DELAY_TO_DISPLAY_SCREEN 5         /* Time to keep display active */
#define NOTIFICATION_REPEAT_COUNT_MAX  2  /* Max notifications to send during notification period */
#define SETUP_SENSORS_DELAY  0.5          /* Delay to read DS18B20 data before initialisation */

#line 60 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void trace(const char * str);
#line 66 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void traceln(const char * str);
#line 72 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
int toFarenheit(int celcius);
#line 78 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isLowVoltage();
#line 82 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isPowerGaugeAlert();
#line 89 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void checkPowerGaugeAvailable();
#line 106 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupPowerGauge();
#line 130 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void notificationTimeEnded();
#line 135 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isNotificationTimerActive();
#line 139 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void cancelNotificationTimer();
#line 145 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupNotification();
#line 150 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupNotificationTimer(int seconds);
#line 157 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void delayBeforeSleepTimerEnded();
#line 162 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isDelayBeforeSleepTimerActive();
#line 166 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void cancelDelayBeforeSleepTimer();
#line 172 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupDelayBeforeSleepTimer(int seconds);
#line 178 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void delayBetweenNotificationsTimerEnded();
#line 183 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isDelayBetweenNotificationsTimerActive();
#line 187 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
bool isDelayBetweenNotificationsTimerInactive();
#line 191 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void cancelDelayBetweenNotificationsTimer();
#line 197 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupDelayBetweenNotificationsTimer(int seconds);
#line 203 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void displayDS18B20Info();
#line 209 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setupSensors();
#line 217 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void readTemperature();
#line 235 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void readSensors();
#line 243 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void deepSleep();
#line 254 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void printSensorsValues();
#line 261 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void notifySensorsValues();
#line 276 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void setup();
#line 295 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void loop();
#line 60 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\DS18B20\\E32\\E32.ino"
void trace(const char * str) {
  if (!disableTraceDisplay) {
    Serial.print(str);
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

void displayDS18B20Info() {
  char str[50];
  sprintf(str, "DS18B20 devices count=%d", sensors.getDeviceCount());
  traceln(str);
}

void setupSensors() {
  // Start the DS18B20 sensor
  sensors.begin();

  delay(1000*SETUP_SENSORS_DELAY);
  setupPowerGauge();
}

void readTemperature() {
  sensors.requestTemperatures();
  uint8_t deviceCount = sensors.getDeviceCount();

  switch (deviceCount) {
    case 0:
      waterTemperature = airTemperature = 0;
      break;
    case 1:
      waterTemperature = sensors.getTempCByIndex(0);
      break;
    case 2:
      waterTemperature = sensors.getTempCByIndex(0);
      airTemperature = sensors.getTempCByIndex(1);
      break;
  }
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
    sprintf(str, "Notify values: sleep delay=%d seconds, water temperature=%d, air temperature=%d, charge=%d, alarm low voltage=%d", 
            TIME_TO_SLEEP, waterTemperature, airTemperature, charge, isLowVoltage());
    traceln(str);
}

void notifySensorsValues() {
    String str = "";
    str += TIME_TO_SLEEP;
    str += ",";
    str += waterTemperature;
    str += ",";
    str += airTemperature;
    str += ",";
    str += charge;
    str += ",";
    str += isLowVoltage() ? "1" : "0";
    //TODO E32 send data
    printSensorsValues();
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
    
  displayDS18B20Info();

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

