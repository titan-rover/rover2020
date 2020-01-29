//#include <Servo.h>
#include <Wire.h>
#include <Stepper.h>
#define SLAVE_ADDRESS 0x05

// change this to the number of steps on your motor
#define STEPS 20

/*Servo pan_servo;
Servo tilt_servo;
int16_t home_val = 0;
int16_t pan = 0;
int16_t tilt = 0;
int tilt_start_angle = 30;
int pan_start_angle = 90;
int tilt_curr_angle = tilt_start_angle;
int pan_curr_angle = pan_start_angle;
int tilt_max_forward_angle = 60;
int tilt_max_back_angle = 0;
int pan_max_right_angle = 30;
int pan_max_left_angle = 150;
int incomingByte = 0; // for incoming serial data*/

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper1(STEPS, 4, 5, 6, 7);
Stepper stepper2(STEPS, 8, 9, 10, 11);

void setup()
{
//   pan_servo.attach(8);
//   tilt_servo.attach(9);
//   tilt_servo.write(tilt_start_angle);
//   pan_servo.write(pan_start_angle);
//   //inputData();
//   Serial.begin(9600);
  // set the speed of the motor to 30 RPMs
  stepper1.setSpeed(60);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
}
void loop()
{
//       // send data only when you receive data:
//   if (Serial.available()) {
//     // read the incoming byte:
//     //Serial.print(Serial.available());
//     incomingByte = Serial.read();
//     // say what you got:
//     Serial.print("I received: ");
//     Serial.println(incomingByte, DEC);
//   }
  stepper1.step(STEPS);
  stepper1.step(-STEPS);
   // stepper2.step(STEPS);
  //stepper2.step(-STEPS);
}
/*void inputData(){
}*/
void receiveData(int byteCount) 
{
     if (Wire.available() == 2)
    {
      pan = Wire.read();
      tilt = Wire.read();
    }
    stepper1.step(STEPS * pan);
    stepper2.step(STEPS * tilt);
//     int16_t recv_home = home_val;
//     int16_t recv_pan = pan;
//     int16_t recv_tilt = tilt;
//     Serial.print(Wire.available());
//     Serial.print("\n");
//     if (Wire.available() == 3) 
//     {
//         recv_home = Wire.read();
//         recv_pan = Wire.read();
//         recv_tilt = Wire.read();
//         if(recv_pan == 255)
//         {
//           recv_pan = (256 - 255) * -1;
//         }
//         else if(recv_tilt == 255)
//         {
//           recv_tilt = (256 - 255) * -1;
//         }
//         Serial.print("\nrecv_home:");
//         Serial.print(recv_home);
//         Serial.print("\nrecv_pan:");
//         Serial.print(recv_pan);
//         Serial.print("\nrecv_tilt:");
//         Serial.print(recv_tilt);
//         Serial.print("\ntilt:");
//         Serial.print(tilt);
//         //Serial.print(mode);
//         //Serial.print(freq);
//         if((home_val == 0 || home_val == 1 && pan >= -1 && pan <= 1 && tilt >= -1 && tilt <= 1))
//         {
//             home_val = recv_home;
//             pan = recv_pan;
//             tilt = recv_tilt;
//             Serial.print("\nhome_val:");
//             Serial.print(home_val);
//             Serial.print("\npan:");
//             Serial.print(pan);
//             Serial.print("\ntilt:");
//             Serial.print(tilt);
//             Serial.print("\n");
//             turn(home_val, pan, tilt);
//         }
//     } 
}
// void turn(int16_t home_val, int16_t pan, int16_t tilt)
// {
//   if(home_val == 1)
//   {
//     pan_servo.write(pan_start_angle);
//     tilt_servo.write(tilt_start_angle);
//   }
//   else if(pan == 1 && pan_curr_angle != pan_max_right_angle)
//   {
//     pan_curr_angle = pan_curr_angle + pan_start_angle;
//     pan_servo.write(pan_curr_angle);
//   }
//   else if(pan == -1 && pan_curr_angle != pan_max_left_angle)
//   {
//     pan_curr_angle = pan_curr_angle - pan_start_angle;
//     pan_servo.write(pan_curr_angle);
//   }
//   else if(tilt == 1 && tilt_curr_angle != tilt_max_forward_angle)
//   {
//     tilt_curr_angle = tilt_curr_angle + tilt_start_angle;
//     tilt_servo. write(tilt_curr_angle);
//   }
//   else if(tilt == -1 && tilt_curr_angle != tilt_max_back_angle)
//   {
//     tilt_curr_angle = tilt_curr_angle - tilt_start_angle;
//     tilt_servo.write(tilt_curr_angle);
//   }
// }
