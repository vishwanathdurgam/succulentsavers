#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Si7021.h>
#include <hp_BH1750.h>
#include <Adafruit_seesaw.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

hp_BH1750 light_sensor;    
Adafruit_Si7021 temp_sensor = Adafruit_Si7021();
Adafruit_seesaw ss;

BLECharacteristic *pCharacteristic;

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  while (!Serial); // Wait for the serial monitor to open
  
  Serial.println("Test");

  BLEDevice::init("Plant Environment Values");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("Server setup complete! Connect your phone to receive data.");

  // Initialize the Si7021 sensor
  if (!temp_sensor.begin()) {  
    Serial.println("Couldn't find temp sensor! Check your wiring.");
    while (1);
  }

  // Initialize the BH1750 sensor
  if (!light_sensor.begin(BH1750_TO_GROUND)) {  
    Serial.println("Couldn't find light sensor! Check your wiring.");
    while (1);
  }

  // Initialize the Seesaw sensor
  if (!ss.begin(0x36)) {  // Default I2C address for Seesaw
    Serial.println("Couldn't find seesaw! Check your wiring.");
    while (1);
  }
}

void loop() {
  // Get the temperature in Celsius
  float ext_tempC = temp_sensor.readTemperature();
  // Print the temperature to the Serial Monitor
  float ext_tempF = ext_tempC * 9.0 / 5.0 + 32.0;
  // Print the temperature to the Serial Monitor
  Serial.print("External Temperature: ");
  Serial.print(ext_tempF);
  Serial.println(" *F");

  float humidity = temp_sensor.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Get the light intensity in Lux
  light_sensor.start();   // starts a measurement
  float lux = light_sensor.getLux();  // waits until a conversion finished
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" Lux");

  // Get the moisture level
  uint16_t moisture = ss.touchRead(0);
  Serial.print("Moisture: ");
  Serial.println(moisture);

  // Get the internal temperature
  float int_tempC = ss.getTemp();
  float int_tempF = int_tempC * 9.0 / 5.0 + 32.0;
  Serial.print("Internal Temperature: ");
  Serial.print(int_tempF);
  Serial.println(" *F");


  // Create a string with the sensor suggestions
    std::string suggestions = "";
  // Humidity check
  // Humidity check
  if (humidity < 30) {
    suggestions += "Humidity is significantly too low.\n";
  } else if (humidity < 40) {
    suggestions += "Humidity is too low.\n";
  } else if (humidity <= 50) {
    suggestions += "Humidity is in the good range.\n";
  } else if (humidity <= 60) {
    suggestions += "Humidity is too high.\n";
  } else {
    suggestions += "Humidity is significantly too high.\n";
  }

  // Internal Temperature check
  if (int_tempF < 55) {
    suggestions += "Internal temperature is significantly too low.\n";
  } else if (int_tempF < 65) {
    suggestions += "Internal temperature is too low.\n";
  } else if (int_tempF <= 75) {
    suggestions += "Internal temperature is in the good range.\n";
  } else if (int_tempF <= 85) {
    suggestions += "Internal temperature is too high.\n";
  } else {
    suggestions += "Internal temperature is significantly too high.\n";
  }

  // External Temperature check
  if (ext_tempF < 62.5) {
    suggestions += "External temperature is significantly too low. Move to a warmer spot immediately.\n";
  } else if (ext_tempF < 72.5) {
    suggestions += "External temperature is too low. Move to a warmer spot.\n";
  } else if (ext_tempF <= 82.5) {
    suggestions += "External temperature is in the good range.\n";
  } else if (ext_tempF <= 92.5) {
    suggestions += "External temperature is too high. Move to a cooler spot.\n";
  } else {
    suggestions += "External temperature is significantly too high. Move to a cooler spot immediately.\n";
  }

  // Light intensity check
  if (lux < 15000) {
    suggestions += "Sunlight is too low. Move the plant to a sunnier spot.\n";
  } else {
    suggestions += "Sunlight is adequate.\n";
  }

  // Soil moisture check
  if (moisture < 280) {
    suggestions += "Soil moisture is way too low. Water the plant immediately.\n";
  } else if (moisture < 400) {
    suggestions += "Soil moisture is too low. Water the plant.\n";
  } else if (moisture <= 650) {
    suggestions += "Soil moisture is in the good range.\n";
  } else if (moisture <= 800) {
    suggestions += "Soil moisture is too high. Stop watering the plant.\n";
  } else {
    suggestions += "Soil moisture is way too high. Dry the plant soil.\n";
  }
  pCharacteristic->setValue(suggestions);
  pCharacteristic->notify();
  
  // Wait for 1 second before the next reading
  delay(1000);
}
