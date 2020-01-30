/*
 Combined code for all sensors
 By: Jithin J Eapen 
 Date: January 15th, 2018
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 Sensors-
 MPL3115A2 library to display the current altitude and temperature
 GUVA-S12SD reading values from Analog Input pin A0 for UV Light Sensing
 5TE Sensor (Decagon Devices)read values from Digital Interrupt Pin 5 using SDI to serial communication
 
 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 
 */
#include <Wire.h>
#include "SparkFunMPL3115A2.h"
#include <Adafruit_MLX90614.h>
#include <dht.h>
#include <JitK30.h> //For CO2 K-30 sensor if wired for i2c
#include <JitAnemometer.h>
#include <ros.h>
#include <science_sensors/sci_msgs.h>

#define SLAVE_ADDRESS 0x06

//Declare Humidity DHT11 sensor and pin number 7
dht DHT;
#define DHT11_PIN 7

//Temperature sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//Pressure sensor
MPL3115A2 myPressure;


ros::NodeHandle nh;
science_sensors::sci_msgs readings;
ros::Publisher sci_sensors("sensors", &readings);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();        // Join i2c bus
  Wire.begin(SLAVE_ADDRESS);
  //Serial.begin(9600);  // Start serial for output
  Serial.begin(57600);  // Start serial for output
  myPressure.begin(); // Get sensor online
  
/*
  // Configure the sensor
  //myPressure.setModeAltimeter(); // Measure altitude above sea level in meters
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa  
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags
  */
  
  //Serial.write("Adafruit MLX90614 test");
  mlx.begin();
  
  nh.initNode();
  nh.advertise(sci_sensors);
}
void loop() {
  Serial.write("START");
  Serial.write("\n");
  
  /* Code for UV Light sensor GUVA-S12SD */
  loopUVLight();  
  
  /* Code for Temperature sensor Melexis MLX90614 */
  //Serial.print("Temperature\n");
  loopTempSensor();
  
  /* Code for 5TE Sensor */
  //loopSoilSensor_5TE();
  
  /* Code for DHT11 Humidity Sensor */
  loopHumidity_DHT11();
  
  /* Code for K-30 CO2 Sensor */
  loopCO2sensor_Jit_K30();

  sci_sensors.publish(&readings);

  Serial.write("END");
  Serial.write("\n");

  nh.spinOnce();
  delay(1000);
}

//Loop code for Melexis MLX90614
void loopTempSensor(){
  readings.ambientC = mlx.readAmbientTempC();
  readings.objectC = mlx.readObjectTempC();
  readings.ambientF = mlx.readAmbientTempF();
  readings.objectF = mlx.readObjectTempF();
}

//Loop code for UV Light Sensor
void loopUVLight(){
  readings.uv_sensor = analogRead(A0);
  readings.voltage = readings.uv_sensor * 3.3 / 1023;
  readings.uv_index = readings.voltage / 0.1;
}

//Loop code for Humidity sensor
void loopHumidity_DHT11() {
  DHT.read11(DHT11_PIN);
  readings.humidity_temperature = DHT.temperature;
  readings.humidity = DHT.humidity;
}

//Loop code for K-30 CO2 sensor
void loopCO2sensor_Jit_K30() {
  readings.co2_ppm = readCO2();
}
