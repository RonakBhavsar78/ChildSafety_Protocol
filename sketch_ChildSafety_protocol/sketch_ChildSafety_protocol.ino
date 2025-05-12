#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"    
#define LM35 A1
#include <SoftwareSerial.h> 

// Motion sensor variable
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

// GSM Location tracker variable
SoftwareSerial mySerial(9, 10);//tx, rx

//Below Variable will append the Lattitude and longitude value later int the program. 
String Link = "The current Location is https://www.google.com/maps/place/"; 
String response = "";
String Longitude = "";
String Latitude = ""; 

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

void setup()
{
  // Setting the baud rate of GSM Module
  mySerial.begin(9600); 
  // Setting the baud rate of Serial Monitor (Arduino)
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  delay(100);
}


void loop()
{
  // Below code of Temprature sensor code to check Child body temp.
  // Convert that ADC Data into voltage

  int adcData = analogRead(LM35);
  float voltage = adcData * (5.0 / 1024.0);
  // Convert the voltage into temperature
  float celsius  = voltage * 100;
  double fahrenheit = (celsius * 9) / 5 + 32;

  Serial.print('\n');
  Serial.print("Temperature: ");
  
  Serial.print(celsius);
  Serial.print(" °C | ");
  
  Serial.print(fahrenheit);
  Serial.println(" °F");
  
  // wait a second between readings
  delay(800); 
 	
  /********************************************************************************************************************/
  // motion sensor code to check if any child in problem then motion sensor axis continuosly increases in (x,y,z) 
  // so we can make a call and send current location through the message on parent's mobile number.

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); 
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data

  // wait a second between readings
  delay(1000);

  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");

  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");

  Serial.print("Axis Z  = ");
  Serial.println(data.Z);

  // wait a second between readings
  delay(2000);

  // Verify Temprature condition to send message to parent's mobile number
   if (celsius >= 36.4 && celsius <= 37.9){
    Serial.println("Your Child is Safe");
  }
  else if(celsius >= 38.9){
    String Tempfever = "Your Child has FEVER!!!";
    MakeCallAndHangupCall();
    SendMessage(Tempfever);
  }
  else{
    String TempNotSafe = "Your Child is NOT SAFE!!!";
    MakeCallAndHangupCall();
    SendMessage(TempNotSafe);
  }

    // wait a second between readings
  delay(800);

  if(data.X >= 500){
    Serial.println(data.X);
    String motionXaxis = "Your Child is Moving!!!"; 
    SendMessage(motionXaxis);
  }  
  
  exit(0);
}

String SIM800_send(String incoming) //Function to communicate with SIM800 module 
{
    mySerial.println(incoming); delay(100); //Print what is being sent to GSM module 
    String result = "";
    while (mySerial.available()) //Wait for result 
    {
    	char letter = mySerial.read();
    	result = result + String(letter); //combine char to string to get result 
    }
}
// Send message to Parent mobile number.
void SendMessage(String message)
{
	Serial.println (message); 
  delay(1000);
  
  response = ""; Latitude=""; Longitude=""; //initialise all string to null
     delay (1000);
     response = SIM800_send("ATE0"); //Disable Echo
     delay (1000);
  mySerial.println("AT+CLBSCFG=4"); 
  delay(5000); //Request for location data 

  while (mySerial.available()) {
  char letter = mySerial.read();
  response = response + String(letter); //Store the location information in string responce 
  }

//use prepare_message funtion to prepare the link with the obtained LAT and LONG co-ordinates 
  delay(1000); 
  prepare_message(); 
  mySerial.println("AT+CMGF=1\r"); //Set the module in SMS mode
  delay(1000);
  mySerial.println("AT+CMGS=\"+918347152300\"\r"); //Send SMS to this number 
  delay(1000);
  mySerial.println(Link); // we have send the string in variable Link 
  delay(1000);
  mySerial.println((char)26);// ASCII code of CTRL+Z - used to terminate the text message 
  delay(1000);

}

// Design message to Send parent mobile number
void prepare_message()
{  
  response = ""; Latitude=""; Longitude=""; //initialise all string to null
  delay (1000);
  response = SIM800_send("ATE0"); //Disable Echo
  delay (1000);
    mySerial.println("AT+CLBSCFG=4");
  delay(5000); //Request for location data 

  while (mySerial.available()) {
  char letter = mySerial.read();
  response = response + String(letter); //Store the location information in string responce 
  }


//Sample Output for AT+CIPGSMLOC=1,1   ==> +CIPGSMLOC: 0,75.802460,26.848892,2019/04/23,08:32:35 //where 26.8488832 is Lattitude and 75.802460 is longitute 
int first_comma = response.indexOf(','); //Find the position of 1st comma 
int second_comma = response.indexOf(',', first_comma+1); //Find the position of 2nd comma 
int third_comma = response.indexOf(',', second_comma+1); //Find the position of 3rd comma 

  for(int i=first_comma+1; i<second_comma; i++) //Values form 1st comma to 2nd comma is Longitude 
    {
    Longitude = Longitude + response.charAt(i); 
    Serial.println(Latitude);
    }

    for(int i=second_comma+1; i<third_comma; i++) //Values form 2nd comma to 3rd comma is Latitude 
    {
    Latitude = Latitude + response.charAt(i); 
    Serial.println(Longitude); 
    }
    
    delay(100);

    Link = Link + Latitude + "," + Longitude; //Update the Link with latitude and Logitude values 
    Serial.println(Link);
}

void MakeCallAndHangupCall()
{
  // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
	mySerial.println("ATD+918347152300;"); 
  // print response over serial port
	Serial.println("Calling ");
	delay(15000);
  mySerial.println("ATH");
	Serial.println("Hangup Call");
}


