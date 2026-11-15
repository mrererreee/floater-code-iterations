/* -------------- NECESSARY LIBRARIES ------------- */ 
#include <FS.h> // file system
#include <Wire.h>
#include <MS5837.h>
#include <LittleFS.h> // file system
#include <Preferences.h>

/* ----------- PIN SETUP ---------*/ 
#define RXD1 15
#define TXD1 16
#define STATE_PIN 17
#define STATE_LED 47
const int PHOTORESISTOR_PIN = 22;

/* ----------- PHOTORESISTOR CALCULATION CONSTANTS ------------- */ 
const float V_REF = 3.3;
const float R_BITS = 12.0;
const float ADC_STEPS = (1 << int(R_BITS)) - 1;
const int threshold = 2000;
int voltage;

/* ----------- INSTANTIATING OBJECTS -------------- */ 
HardwareSerial blueTooth(2);
Preferences prefs;
MS5837 sensor;

/* ------------- SETTING UP FILE IN LITTLEFS ------------ */
const char* LOG_PATH = "/samplelog.txt"; // file name
// append line function, to add samples as long as it continues
void appendLine(const char* path, const String& line) { 
  File f = LittleFS.open(path, LittleFS.exists(path) ? FILE_APPEND : FILE_WRITE);
  if (!f) {
    Serial.println("Append failed");
    return;
  }
  f.println(line);
  f.close();
}


void setup() {

  Serial.begin(115200);

  pinMode(STATE_PIN, INPUT);
  pinMode(STATE_LED, OUTPUT);
  Wire.begin();

  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
  blueTooth.begin(9600, SERIAL_8N1, RXD1, TXD1);

  /*
  usb start-up
  uint32_t tStart = millis();
  while(!Serial && (millis() - tStart < 2000)) {}
  */

  Serial.println("==== LittleFS WRITER ====");

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    while(1) {}
  }

  while (!sensor.init()) {
    Serial.println("init failed. are sda/scl connected correctly");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  // if file is new/empty, write a header row
  if(!LittleFS.exists(LOG_PATH) || LittleFS.open(LOG_PATH, FILE_READ).size() == 0) {
    appendLine(LOG_PATH,"time\t pressure\t depth\t temp\t light\t");
  }

  size_t used = LittleFS.usedBytes();
  size_t total = LittleFS.totalBytes();
  Serial.printf("Mounted. Used %u / %u bytes\n", (unsigned)used, (unsigned)total);
  Serial.printf("Logging to %s every 5s. Open Serial Monitor to watch. \n", LOG_PATH);

}

void loop() {
  // put your main code here, to run repeatedly:
  sensor.read();

  voltage = analogRead(PHOTORESISTOR_PIN);
  float voltageMeasure = (voltage / ADC_STEPS) * V_REF;

  int state = digitalRead(STATE_PIN);

  float pressure = sensor.pressure();
  float temperature = sensor.temperature();
  float depthMBAR = sensor.depth();


  static uint32_t t0 = 0;

  /*while(!state) //&& boots > 0)
  {
    if(millis() - t0 >= 5000) {
      t0 = millis();
      String line = String(millis() + " " + String(voltageMeasure) +"\t" + String(pressure) + "\t" + String(temperature) + "\t" + String(depthMBAR));
      appendLine(LOG_PATH, line);
      Serial.println("APPEND: " + line);
    }
  } */

      // reads all information from depth sensor
    if (state && blueTooth.available())
    {
      digitalWrite(STATE_LED, state);

      blueTooth.print("Time: ");
      blueTooth.print(millis() / 5000);
      blueTooth.print(" s ");

      blueTooth.print("Pressure: ");
      blueTooth.print(pressure);
      blueTooth.print(" mbar");
    
      blueTooth.print(" Temperature: ");
      blueTooth.print(temperature);
      blueTooth.print(" deg C");
    
      blueTooth.print(" Depth: ");
      blueTooth.print(depthMBAR);
      blueTooth.print(" m");
  
      // prints voltage calculation
      blueTooth.print(" ");
      blueTooth.print(voltageMeasure);
      blueTooth.print(" V");
      blueTooth.println();

      delay(5000);
    }

    else 
    {
      digitalWrite(STATE_LED, state);
      if(millis() - t0 >= 5000) {
        t0 = millis();
        String line = String(millis() + " " + String(voltageMeasure) +"\t" + String(pressure) + "\t" + String(temperature) + "\t" + String(depthMBAR));
        appendLine(LOG_PATH, line);
        Serial.println("APPEND: " + line);
      }
    }

  // what do i want to happen if state is 0?




}
