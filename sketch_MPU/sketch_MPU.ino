#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"    
#define LM35 A1 

MPU6050 mpu;


int16_t ax, ay, az;
int16_t gx, gy, gz;

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
}


void loop()
{
   // Temprature sensor code.
	
   // Convert that ADC Data into voltage
   int adcData = analogRead(LM35);
   float voltage = adcData * (5.0 / 1024.0);
   // Convert the voltage into temperature
   float temperature = voltage * 100;
   // Print the temperature data
   Serial.print("Temperature: ");
   Serial.print(temperature);
   Serial.println("*C");
   // wait a second between readings
   delay(800); 
 	

  // motion sensor code.
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); 
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data

  // wait a second between readings
  delay(500);

  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");

  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");

  Serial.print("Axis Z  = ");
  Serial.println(data.Z);
  
}
