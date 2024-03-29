 /*
 *File:        leds.ino
 *Author:      Chary Vielma
 *Email:       chary.vielma@csu.fullerton.edu
 *Description: Program receives 2 byte values via the i2c bus to indicate mode and frequency.
 *             LED strip on Rover will be lit according to these values (e.g. (1, 3) for
 *             green and purple). Must use with LED library found at:
 *             https://github.com/pololu/pololu-led-strip-arduino
 */
 
#include <Wire.h>
#include <PololuLedStrip.h>

#define LED_COUNT 6
#define SLAVE_ADDRESS 0x04 // Arduino i2c Slave address

PololuLedStrip<12> ledStrip; // Create an ledStrip object
rgb_color colors[LED_COUNT]; // Must be multiple of 4

struct roverColors {
  uint8_t rgbVals[3];
} myColors[] = {
  { {255, 0, 0} }, // red 0
  { {0, 255, 0} }, // green 1 
  { {0, 0, 255} }, // blue 2
  
  { {255, 0, 255} }, // purple 3
  { {255, 255, 255} }, // white 4
  { {255, 60, 0} }, // orange 5  
  { {0, 0, 0} } // off 6
};

uint16_t mode = 10;
uint16_t freq = 10;

// Voltage reader variables
double Vout; // voltage at pin A0
double Vin = 0.0; // voltage being measured, max = 25v
double factor = 5.0;
double Vcc = 5.00; // this is the Arduino's Vcc, measure and adjust if necessary

void setup()
{ 
    Wire.begin(SLAVE_ADDRESS);    // join i2c bus with address #4
    Wire.onReceive(receiveData);  // register event
    csuf();
    //Serial.begin(9600);
}

void loop()
{
/*    Vout = analogRead(0); // read the adjusted voltage 0 to 1023
    Vout =(Vout/1023)*Vcc; // convert to a voltage 0 - 5 volts
    Vin = Vout * factor; // convert to the voltage being measured
*/
}

void receiveData(int byteCount) 
{
    //LIGHTS
    uint16_t recv_mode = mode;
    uint16_t recv_freq = freq;
    if (Wire.available() == 2) 
    {
        recv_mode = Wire.read();
        recv_freq = Wire.read();
        //Serial.print(mode);
        //Serial.print(freq);
       
        if((mode >= 0 && mode <= 10 && freq >= 0 && freq <= 10) && (recv_mode != mode || recv_freq != freq))
        {
            mode = recv_mode;
            freq = recv_freq;
            set(mode, freq);
        }

        // VOLTAGE READER
        //Vout = analogRead(0); // read the adjusted voltage 0 to 1023
        //Vout =(Vout/1023)*Vcc; // convert to a voltage 0 - 5 volts
        //Vin = Vout * factor; // convert to the voltage being measured
        // TODO: convert to byte array and pad with zeros EX: Wire.write(sample, BUFFER_SIZE);
    } 
}

void set(uint16_t mode, uint16_t freq)
{
    if (mode == 7 && freq == 7)
    {
        found();
    }
    
    else if (mode == 10 && freq == 10)
    {
        csuf();
    }
    
    else
    {
        uint16_t segLen = LED_COUNT;
        for(uint16_t i = 0; i < segLen; i++)
        {
            colors[i].red = myColors[freq].rgbVals[0], colors[i].green = myColors[freq].rgbVals[1], colors[i]. blue = myColors[freq].rgbVals[2];
            colors[i + segLen].red = myColors[mode].rgbVals[0], colors[i + segLen].green = myColors[mode].rgbVals[1], colors[i + segLen].blue = myColors[mode].rgbVals[2];
            colors[i + 2*segLen].red = myColors[mode].rgbVals[0], colors[i + 2*segLen].green = myColors[mode].rgbVals[1], colors[i + 2*segLen].blue = myColors[mode].rgbVals[2];
            colors[i + 3*segLen].red = myColors[freq].rgbVals[0], colors[i + 3*segLen].green = myColors[freq].rgbVals[1], colors[i + 3*segLen].blue = myColors[freq].rgbVals[2];
        }
        ledStrip.write(colors, LED_COUNT);
    }
}

// From PololuLed library
void found()
{
    for(int i = 0; i < 150; i++)
    {    
        byte time = millis() >> 2;
        for (uint16_t i = 0; i < LED_COUNT; i++)
        {
            byte x = time - 8*i;
            colors[i] = rgb_color(0, 255 - x, 0);
        }
        // Write the colors to the LED strip.
        ledStrip.write(colors, LED_COUNT);
        delay(100);
    }
}

void csuf()
{
    int chaseSize = 2;
    int current = 0;
    for(int oddLed = 0; ; oddLed++)
    {
        if (mode != 10 || freq != 10)
        {
            break;
        }
        for(int i = 0; i < LED_COUNT; i++)
        {
            current = oddLed % chaseSize;
            colors[i] = (i % chaseSize == current) ? rgb_color(225,80, 0) : rgb_color(0, 25, 90);
            ledStrip.write(colors, LED_COUNT);
            delay(50);
        }
    current++;
    }
}
