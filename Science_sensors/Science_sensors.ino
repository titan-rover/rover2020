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
#include <SDISerial.h>
#include <dht.h>
#include <JitK30.h> //For CO2 K-30 sensor if wired for i2c
#include <JitAnemometer.h>
#include <ros.h>
#include <science_sensors/sci_msgs.h>

#define SLAVE_ADDRESS 0x06

//For 5TE device, in order to recieve data you must choose a pin that supports interupts
#define DATALINE_PIN_5TE 2
#define INVERTED_5TE 1

//Declare Humidity DHT11 sensor and pin number 7
dht DHT;
#define DHT11_PIN 7

//Temperature sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//Pressure sensor
MPL3115A2 myPressure;

//SDI Serial communication for 5TE Device for testing soil moisture
SDISerial sdi_serial_connection(DATALINE_PIN_5TE, INVERTED_5TE);

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
  
  //Setup 5TE
  //setup5TE();
  nh.initNode();
  nh.advertise(sci_sensors);
  //Wire.onRequest(requestEvent);
}
void loop() {
  Serial.write("START");
  Serial.write("\n");
  //noInterrupts();
  /* Code for Pressure/Altitude sensor MPL3115A2 */  
  //loopPressure();
  
  /* Code for UV Light sensor GUVA-S12SD */
  //Serial.print("UV\n");
  loopUVLight();  
  
  /* Code for Temperature sensor Melexis MLX90614 */
  //Serial.print("Temperature\n");
  loopTempSensor();
  
  /* Code for 5TE Sensor */
  //Serial.print("Soil\n");
  //loopSoilSensor_5TE();
  
  /* Code for DHT11 Humidity Sensor */
  //Serial.print("Humidity\n");
  loopHumidity_DHT11();
  
  /* Code for K-30 CO2 Sensor */
  //Serial.print("CO2\n");
  loopCO2sensor_Jit_K30();

  sci_sensors.publish(&readings);
  /* Loop code for Anemometer */
  //loopWindSpeed();

  //printArray();
  //interrupts();
  Serial.write("END");
  Serial.write("\n");

  nh.spinOnce();
  delay(1000);
}
//Get Measurement from 5TE Device by sending SDI queries
char* get_measurement(){
  char* service_request = sdi_serial_connection.sdi_query("?M!",1000); //sending our query via connection
  char* service_request_complete = sdi_serial_connection.wait_for_response(1000); //waiting for the response from the sensor
  return sdi_serial_connection.sdi_query("?D0!", 1000); //it'll return as soon as we get a clean response  
}

//Setup code for 5TE
void setup5TE(){
  sdi_serial_connection.begin(); // start our SDI connection
  
  //Serial.println("5TE INITIALIZED"); // startup string echo'd to our uart
  
  //delay(3000); // startup delay to allow sensor to powerup and output its DDI serial string
}

char* getCharFromFloat(float num){
  num = num * 100;
  int abc = (int)num;
  char* a = (char*)malloc(256);
  sprintf(a, "%d",abc );
  return a;
}

char* getCharFromInteger(int num){  
  char* a = (char*)malloc(256);
  sprintf(a, "%d",num );
  return a;
}

char* getCharFromLong(float num){
  num = num * 100;
  long abc = (long)num;
  char* a = (char*)malloc(256);
  sprintf(a, "%6ld",abc );
  return a;
}


//Loop code for Pressure sensor
//void loopPressure() {
//  /*  float altitude = myPressure.readAltitude();
//  Serial.print("Altitude(m):");
//  Serial.print(altitude, 2);
//  altitude = myPressure.readAltitudeFt();
//  Serial.print(" Altitude(ft):");
//  Serial.print(altitude, 2);*/
//  
//  float pressure = myPressure.readPressure();
//  //Serial.println("MPL3115A2 Pressure(Pa):");
//  char* tempPressure = getCharFromLong(pressure);
//  Serial.write(tempPressure);  
//  Serial.write('\n');
//  free(tempPressure);
//  
//  //float temperature = myPressure.readTemp();
//  //Serial.print(" Temp(c):");
//  //Serial.print(temperature, 2);
//  
//  float temperature = myPressure.readTempF();
//  //Serial.print(" Temp(f):");
//  //Serial.println(temperature,2);
//  char* temp = getCharFromFloat(temperature);
//   
//  Serial.write(temp);
//  Serial.write('\n');
//  free(temp);
//  //Serial.println();
//}

//Loop code for Melexis MLX90614
void loopTempSensor(){
  readings.ambientC = mlx.readAmbientTempC();
  readings.objectC = mlx.readObjectTempC();
  readings.ambientF = mlx.readAmbientTempF();
  readings.objectF = mlx.readObjectTempF();
  /*char* ambTempC = getCharFromFloat(mlx.readAmbientTempC());
  Serial.write("Ambient Temperature = ");
  Serial.write(ambTempC);
  Serial.write("*C");
  Serial.write('\n');
  //strcpy(readings[3], ambTempC);
  temp_readings[0] = ambTempC;
  ambC = ambTempC;
  free(ambTempC);
  
  char* objTempC = getCharFromFloat(mlx.readObjectTempC());
  Serial.write("Object Temperature = ");
  Serial.write(objTempC);
  Serial.write("*C");
  Serial.write('\n');
  //strcpy(readings[4], objTempC);
  temp_readings[1] = objTempC;
  objC = objTempC;
  free(objTempC);
  ros
  
  char* ambTempF = getCharFromFloat(mlx.readAmbientTempF());
  Serial.write("Ambient Temperature = "); 
  Serial.write(ambTempF);
  Serial.write("*F"); 
  Serial.write('\n');
  //strcpy(readings[5], ambTempF);
  temp_readings[2] = ambTempF;
  ambF = ambTempF;
  free(ambTempF);
  
  char* objTempF = getCharFromFloat(mlx.readObjectTempF());
  Serial.write("Object Temperature = ");
  Serial.write(objTempF);
  Serial.write("*F");
  Serial.write('\n');
  //strcpy(readings[6], objTempF);
  temp_readings[3] = objTempF;
  objF = objTempF;
  free(objTempF);*/  
}

//Loop code for UV Light Sensor
void loopUVLight(){
  readings.uv_sensor = analogRead(A0);
  readings.voltage = readings.uv_sensor * 3.3 / 1023;
  readings.uv_index = readings.voltage / 0.1;
  /*int sensor = analogRead(A0);
  float voltage = sensor * 3.3 / 1023;
  float uvindex = voltage / 0.1;
  
  Serial.write("UV Sensor - ");
  char* tempSensor = getCharFromInteger(sensor);
  appendToChar(tempSensor);
  Serial.write(tempSensor);
  Serial.write('\n');
  readings[0] = tempSensor;
  uv_readings[0] = tempSensor;
  uv_sensor = tempSensor;
  //free(tempSensor);
  
  Serial.print("Voltage - ");
  char* tempVolt = getCharFromFloat(voltage);
  appendToChar(tempVolt);
  Serial.write(tempVolt); 
  Serial.write('\n');
  uv_readings[1] = tempVolt;
  readings[1] = tempVolt;
  uv_volt = tempVolt;
  //free(tempVolt);
  
  Serial.print("UV Index - ");  
  char* tempUvIndex = getCharFromFloat(uvindex);
  appendToChar(tempUvIndex);
  Serial.write(tempUvIndex);
  Serial.write('\n');
  readings[2] = tempUvIndex;
  uv_readings[2] = tempUvIndex;
  uv_index = tempUvIndex;
  //free(tempUvIndex);*/
}

//Loop code for Humidity sensor
void loopHumidity_DHT11() {
  readings.humidity_temperature = DHT.temperature;
  readings.humidity = DHT.humidity;
  /*int chk = DHT.read11(DHT11_PIN);
  
  Serial.print("Temperature in C = ");
  char* temp = getCharFromFloat(DHT.temperature);
  Serial.write(temp);
  Serial.write('\n');
  //strcpy(readings[10], temp);
  humid_readings[0] = temp;
  humid_temp = temp;
  free(temp);
  
  Serial.print("Humidity in % = ");
  char* tempHumidity = getCharFromFloat(DHT.humidity);
  Serial.write(tempHumidity);
  Serial.write('\n');
  //strcpy(readings[11], tempHumidity);
  uv_readings[1] = tempHumidity;
  humid = tempHumidity;
  free(tempHumidity);
  
  //delay(1000);*/
}

//Loop code for 5TE Decagon Soil sensor
/*void loopSoilSensor_5TE() {
  int plusCounter = 0;
  int readCounter = 0;
  char* response = get_measurement();
  if (response) {
    char* something = strtok(response, "+");
    while(something != NULL) {
      if(plusCounter != 0) {
        if(readCounter == 0)
        {
          Serial.write("Raw Dielectric Output (VWC) = ");
          Serial.write(something);
          Serial.write('\n');
          //strcpy(readings[7], something);
          readings[3] = something;
          soil_readings[0] = something;
          vwc = something;
          readCounter++;  
        }
        else if(readCounter == 1)
        {
          Serial.write("Raw Electrical Conductivity = ");
          Serial.write(something);
          Serial.write('\n');
          //strcpy(readings[8], something);
          readings[3] = something;
          soil_readings[4] = something;
          ec = something;
          readCounter++;
        }
        else if(readCounter == 2)
        {
          Serial.write("Raw Temperature in Celsius = ");
          Serial.write(something);
          Serial.write('\n');
          //strcpy(readings[9], something);
          readings[3] = something;
          soil_readings[5] = something;
          rawTemp = something;
          readCounter++;
        }
        //Serial.write(something);
        //Serial.write('\n');     
      }
      something = strtok (NULL, "+"); //Increments to next value after +
      plusCounter++;
    }//While loop ends  
    
  }//If response ends
}*/

//Loop code for K-30 CO2 sensor
void loopCO2sensor_Jit_K30() {
  readings.co2_ppm = readCO2();
  /*int co2Value = readCO2();
  
  if (co2Value <= 0) //Means Checksum failed / Communication failure
  {
    co2Value = -1;
  }  
  char* tempCO2 = getCharFromInteger(co2Value);
  Serial.write("Co2 ppm = ");
  Serial.write(tempCO2);
  Serial.write('\n');
  //strcpy(readings[12], tempCO2);
  readings[2] = tempCO2;
  co2_readings[0] = tempCO2;
  ppm = tempCO2;
  //free(tempCO2);
  
  //delay(2000);*/
}

//Loop code for Anemometer
/*void loopWindSpeed() {
  
  char* tempWindSpeed = getCharFromFloat(readWindSpeed()); //Check header file for detailed code
  Serial.write(tempWindSpeed);
  Serial.write('\n');
  free(tempWindSpeed);
}*/

///*void requestEvent()
//{
//    char* tempPtr;
//    char uv[6];
//    char temp[4];
//    char soil[3];
//    char humidity[2];
//    char co2[1];
//
//// Writng UV readings to bus
//    /*for(int i = 0; i < 3; i++)
//    {
//      tempPtr = uv_readings[i];
//      for(int j = 0; j < strlen(tempPtr); j++)
//      {
//        uv[j] = tempPtr[j];
//      }
//      Wire.write(uv, 6);
//      free(uv);
//    }*/
//    Wire.write(uv_sensor, 6);
//    free(uv_sensor);
//    Wire.write(uv_volt, 6);
//    free(uv_volt);
//    Wire.write(uv_index, 6);
//    free(uv_index);
//
//    /*Wire.write(ambC, 4);
//    free(ambC);
//    Wire.write(objC, 4);
//    free(objC);
//    Wire.write(ambF, 4);
//    free(ambF);
//    Wire.write(objF, 4);
//    free(objF);*/
//
//    /*Wire.write(vwc, 4);
//    free(vwc);
//    Wire.write(ec, 4);
//    free(ec);
//    Wire.write(rawTemp, 4);
//    free(rawTemp);*/
//
//    /*Wire.write(humid_temp,4);
//    free(humid_temp);
//    Wire.write(humid, 4);
//    free(humid);*/
//
//    /*Wire.write(ppm, 4);
//    free(ppm);*/
//
//    
//// Writing temperature readings to bus
///*    for(int i = 0; i < 4; i++)
//    {
//      tempPtr = temp_readings[i];
//      for(int j = 0; j < strlen(tempPtr); j++)
//      {
//         temp[i] = tempPtr[j];
//      }
//      Wire.write(temp, 4);
//      free(temp);
//    }*/
//
//// Writing soil readings to bus
///*    for(int i = 0; i < 3; i++)
//    {
//      tempPtr = soil_readings[i];
//      for(int j = 0; j < strlen(tempPtr); j++)
//      {
//         soil[i] = tempPtr[j];
//      }
//      Wire.write(temp, 4);
//      free(soil);
//    }*/
//
//// Writing humidity readings to bus
///*    for(int i = 0; i < 2; i++)
//    {
//      tempPtr = humid_readings[i];
//      for(int j = 0; j < strlen(tempPtr); j++)
//      {
//         humidity[i] = tempPtr[j];
//      }
//      Wire.write(humidity, 4);
//      free(humidity);
//    }*/
//
//// Writing CO2 readings to bus
///*    tempPtr = co2_readings[0];
//    for(int i = 0; i < strlen(tempPtr); i++)
//    {
//      co2[i] = tempPtr[i];
//    }
//    Wire.write(co2, 4);
//    free(co2);*/
//}

/*void printArray()
{
  for(int i = 0; i < 13; i++)
  {
    Serial.print(readings[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
}*/

///*void appendToChar(char* cTemp)
//{
//  byte temp[6];
//  char if_size_is_3[4] = ".00";
//  char if_size_is_4[3] = ".0";
//
//  if(strlen(cTemp) == 3)
//  {
//    strcat(cTemp, if_size_is_3);
//  }
//  else if(strlen(cTemp) == 4)
//  {
//    strcat(cTemp, if_size_is_4);
//  }
//  /*Serial.print(cTemp);
//  Serial.print(" ");
//  Serial.print(strlen(cTemp));
//  Serial.print("\n");*/
//
//  /*for(unsigned int i = 0; i < 6; i++)
//  {
//    temp[i] = cTemp[i];
//  }
//  for(unsigned int j = 0; j < 6; j++)
//  {
//    Serial.print(temp[j]);
//  }
//  Serial.print("\n");*/
//}
