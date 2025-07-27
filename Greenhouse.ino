#include "arduino_secrets.h"

#include <DHT.h>         // Library for DHT temperature/humidity sensor
#include <LowPower.h>    // Library for putting the Arduino into low power sleep modes

// âââ Pin Assignments âââ
#define DHTPIN 2         // DHT11 sensor data pin connected to digital pin 2
#define DHTTYPE DHT11    // Specify DHT sensor model: DHT11
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// Moisture sensor analog pins (up to 3 sensors)
const int moisturePins[] = {A0, A2, A3}; 
const int numMoistureSensors = 3;  // Number of connected soil moisture sensors

// Other analog/digital pin assignments
const int lightPin        = A1;  // Light-dependent resistor (LDR) analog input
const int pumpPin         = 3;   // PWM pin for water pump
const int fanPin          = 6;   // PWM pin for cooling fan
const int lightControlPin = 5;   // Digital output to control LED lights

// âââ Thresholds for triggering devices âââ
const int   moistureLowThreshold   = 400;  // Below this value, soil is considered dry
const int   moistureHighThreshold  = 500;  // Above this value, soil is sufficiently wet
const float fanOnTemp              = 40.0; // Fan turns on above this temperature (Â°C)
const float fanOffTemp             = 35.0; // Fan turns off below this temperature (Â°C)
const float humidityOnThreshold    = 65.0; // Fan also turns on above this humidity (%)
const float humidityOffThreshold   = 55.0; // Fan turns off below this humidity (%)
const int   lightDarkThreshold     = 300;  // Turn light on below this light sensor value
const int   lightBrightThreshold   = 400;  // Turn light off above this light sensor value

// âââ PWM Levels âââ
const uint8_t pumpPWM = 255;  // Max PWM for pump
const uint8_t fanPWM  = 255;  // Max PWM for fan

// âââ Device State Flags âââ
bool fanState   = false;  // Tracks if fan is currently on
bool lightState = false;  // Tracks if light is currently on

// âââ Power-saving Helper Functions âââ
void sleepSeconds(int secs) {
 int cycles = (secs + 7) / 8;  // Convert seconds to 8-second sleep cycles
 for (int i = 0; i < cycles; i++) {
   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Sleep for 8 seconds
 }
}

void sleepMinutes(int mins) {
 int totalSecs = mins * 60;
 int cycles    = (totalSecs + 7) / 8;
 for (int i = 0; i < cycles; i++) {
   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Sleep for total duration
 }
}

// âââ Sensor Reading Function âââ
int readAverageMoisture() {
 long total = 0;
 for (int i = 0; i < numMoistureSensors; i++) {
   total += analogRead(moisturePins[i]);  // Sum moisture readings
 }
 return total / numMoistureSensors;  // Return average
}

// âââ Arduino Setup âââ
void setup() {
 // Set all sensor pins as inputs
 for (int i = 0; i < numMoistureSensors; i++) {
   pinMode(moisturePins[i], INPUT);
 }
 pinMode(lightPin, INPUT);
 pinMode(pumpPin, OUTPUT);
 pinMode(fanPin, INPUT);
 pinMode(lightControlPin, OUTPUT);

 // Ensure all devices are initially off
 analogWrite(pumpPin, 0);
 analogWrite(fanPin,  0);
 digitalWrite(lightControlPin, LOW);

 Serial.begin(9600);   // Start serial communication
 dht.begin();          // Start DHT sensor
 Serial.println("Smart Greenhouse Starting...");
}

// âââ Main Control Loop âââ
void loop() {
 // Read all sensors
 int   moisture = readAverageMoisture();
 int   lightVal = analogRead(lightPin);
 float temp     = dht.readTemperature();
 float hum      = dht.readHumidity();

 // Error handling for DHT sensor
 if (isnan(temp) || isnan(hum)) {
   Serial.println("DHT read failed!");
   temp = hum = 0;
 }

 // Output sensor values to serial monitor
 Serial.print("Soil: ");     Serial.println(moisture);
 Serial.print("Light: ");     Serial.println(lightVal);
 Serial.print("Temp: ");     Serial.println(temp);
 Serial.print("Humidity: "); Serial.println(hum);

 // âââ Light Control âââ
 if (!lightState && lightVal < lightDarkThreshold) {
   digitalWrite(lightControlPin, HIGH);  // Turn on lights
   lightState = true;
   Serial.println("LIGHT ON");
 }
 else if (lightState && lightVal > lightBrightThreshold) {
   digitalWrite(lightControlPin, LOW);   // Turn off lights
   lightState = false;
   Serial.println("LIGHT OFF");
 }

 // âââ Fan Control (with hysteresis) âââ
 bool shouldFan = (temp > fanOnTemp) || (hum > humidityOnThreshold);
 if (!fanState && shouldFan) {
   delay(500);  // Small delay before activating fan
   analogWrite(fanPin, fanPWM);
   fanState = true;
   Serial.println("Fan ON");
 }
 else if (fanState && (temp < fanOffTemp) && (hum < humidityOffThreshold)) {
   analogWrite(fanPin, 0);  // Turn off fan
   fanState = false;
   Serial.println("Fan OFF");
 }

 Serial.print("Fan should be: ");
 Serial.println(shouldFan ? "ON" : "OFF");

 // âââ 15-Second Intensive Monitoring Loop âââ
 bool outOfLine = (moisture < moistureLowThreshold) || shouldFan;
 if (outOfLine) {
   Serial.println("Entering 15s check loopâ¦");

   do {
     // Re-read all sensor values
     moisture = readAverageMoisture();
     temp     = dht.readTemperature();
     hum      = dht.readHumidity();
     lightVal = analogRead(lightPin);
     if (isnan(temp) || isnan(hum)) { temp = hum = 0; }

     // Debug output during the loop
     Serial.print("  chk Soil: ");     Serial.println(moisture);
     Serial.print("  chk Temp: ");     Serial.println(temp);
     Serial.print("  chk Humidity: "); Serial.println(hum);
     Serial.print("  chk Light: ");    Serial.println(lightVal);

     // Water pump activation if soil is too dry
     if (moisture < moistureLowThreshold) {
       Serial.println("  Pump ON (1s)");
       analogWrite(pumpPin, pumpPWM);
       delay(1000);  // Run pump for 1 second
       analogWrite(pumpPin, 0);
     }

     // Light control within the 15s loop
     if (lightVal < lightDarkThreshold && !lightState) {
       Serial.println("  LIGHT ON");
       digitalWrite(lightControlPin, HIGH);
       lightState = true;
     } else if (lightVal > lightBrightThreshold && lightState) {
       Serial.println("  LIGHT OFF");
       digitalWrite(lightControlPin, LOW);
       lightState = false;
     }

     // Fan control within the loop
     shouldFan = (temp > fanOnTemp) || (hum > humidityOnThreshold);
     if (!fanState && shouldFan) {
       Serial.println("  Fan ON");
       analogWrite(fanPin, fanPWM);
       fanState = true;
     }
     else if (fanState && (temp < fanOffTemp) && (hum < humidityOffThreshold)) {
       Serial.println("  Fan OFF");
       analogWrite(fanPin, 0);
       fanState = false;
     }

     sleepSeconds(15); // Sleep between checks to save power

     // Re-check whether conditions are still out of range
     outOfLine = (moisture < moistureHighThreshold) || shouldFan;
   } while (outOfLine); // Repeat until conditions normalize

   Serial.println("Conditions back in range");
 }

 // If all conditions are normal, sleep for ~15 minutes
 Serial.println("Sleeping ~15 minutesâ¦");
 delay(1000);  // Short delay before long sleep
 sleepMinutes(15);
}
