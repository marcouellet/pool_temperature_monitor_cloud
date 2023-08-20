# 1 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino"
# 2 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2

# 4 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2

# 6 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2
# 7 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2

# 9 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2
# 10 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2

# 12 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino" 2




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
int my_bauds;
int cpufreqs[6] = {240, 160, 80, 40, 20, 10};
//int i = 0;
uint16_t prescaler = 80; // Between 0 and 65 535
uint32_t cpu_freq_mhz = 80; // Reduce to 80 mhz (default is 240mhz)
int threshold = 1000000; // 64 bits value (limited to int size of 32bits)
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
# 67 "E:\\Documents\\Flutter\\pool_temperature_monitor_cloud\\sensor_board\\cc1101\\cc1101.ino"
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
  Wire.beginTransmission(0x32 /* Aliexpress clone*/);
  error = Wire.endTransmission();
    if (error == 0 || error == 5) {
      sprintf(str, "MAX17043 device found at address 0x%02X", 0x32 /* Aliexpress clone*/);
      traceln(str);
      isPowerGaugeAvailable = true;
    }
    else {
      sprintf(str, "Error %d while accessing MAX17043 device at address 0x%02X", error, 0x32 /* Aliexpress clone*/);
      traceln(str);
    }
}

void setupPowerGauge() {
  Wire.begin (21, 15);
  pinMode (21, 0x02);
  pinMode (15, 0x02);
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
  setupNotificationTimer(30 /* Time ESP32 stay awaken to send notifications */);
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

  delay(1000*0.5 /* Delay to read DS18B20 data before initialisation */);
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

    sprintf(str, "Going to deep sleep for %d seconds", 60 * 5 /* Time ESP32 will stay in deep sleep before awakening (in seconds) */);
    traceln(str);

    ESP.deepSleep(60 * 5 /* Time ESP32 will stay in deep sleep before awakening (in seconds) */ * 1000000ULL /* Conversion factor for micro seconds to seconds */, RF_DEFAULT);
}

void printSensorsValues() {
    char str[150];
    sprintf(str, "Notify values: sleep delay=%d seconds, water temperature=%d, air temperature=%d, charge=%d, alarm low voltage=%d",
            60 * 5 /* Time ESP32 will stay in deep sleep before awakening (in seconds) */, waterTemperature, airTemperature, charge, isLowVoltage());
    traceln(str);
}

void notifySensorsValues() {
    String str = "";
    str += 60 * 5 /* Time ESP32 will stay in deep sleep before awakening (in seconds) */;
    str += ",";
    str += waterTemperature;
    str += ",";
    str += airTemperature;
    str += ",";
    str += charge;
    str += ",";
    str += isLowVoltage() ? "1" : "0";
    //TODO CC1101 send data
    printSensorsValues();
}

void setup() {
  char str[150];
  Serial.begin(my_bauds);
  delay(500);

  traceln("Setup started");

  sprintf(str, "Baud rate = %d seconds", my_bauds);
  traceln(str);

  setupSensors();
  readSensors();

  displayDS18B20Info();

  setupNotification();
  //TODO setupC1101Service(); 
}

void loop() {

  if (deviceConnected && !notificationTimeOut && notificationRepeatCount < 2 /* Max notifications to send during notification period */) {
    if (delayBetweenNotificationsTimeOut) {
      notificationRepeatCount++;
      notifySensorsValues();
      traceln("Device notified");
    }
    if (notificationRepeatCount == 0 && isDelayBetweenNotificationsTimerInactive()) {
      delayBetweenNotificationsTimeOut = true; // No delay for first notification   
    } else if (delayBetweenNotificationsTimeOut && 2 /* Max notifications to send during notification period */ > 1) {
      setupDelayBetweenNotificationsTimer(5 /* Wait time between each notification send during notification period */);
    }
  }

  if (notificationTimeOut || notificationRepeatCount >= 2 /* Max notifications to send during notification period */) {

      cancelDelayBetweenNotificationsTimer();
      cancelNotificationTimer();

      if (!deepSleepRequested) {
        if (notificationTimeOut) {
          traceln("Device notification timeout");
        } else {
          traceln("Device notification done");
        }
        setupDelayBeforeSleepTimer(5 /* Time ESP32 stay awaken before gooing to deep sleep after notification period */);
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
