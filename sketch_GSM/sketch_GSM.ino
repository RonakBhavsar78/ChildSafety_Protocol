
#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10);//tx, rx

String Link = "The current Location is https://www.google.com/maps/place/"; //we will append the Lattitude and longitude value later int the program 
String responce = "";
String Longitude = "";
String Latitude = ""; 

char msg;
char call;

void setup()
{
// Setting the baud rate of GSM Module
mySerial.begin(9600); 
// Setting the baud rate of Serial Monitor (Arduino)
Serial.begin(9600); 
Serial.println("GSM SIM900A BEGIN");
Serial.println("Enter character for control option:");
Serial.println("h : to disconnect a call");
Serial.println("i : to receive a call");
Serial.println("s : to send message");
Serial.println("r : ReceiveMessage");
Serial.println("c : to make a call");
Serial.println("e : to redial");
Serial.println();
delay(100);
}

void loop()
{
	if (Serial.available()>0)
	switch(Serial.read())
	{
	case 's':
	SendMessage();
	break;
	case 'r':
	ReceiveMessage();
	break;
	case 'c':
	MakeCall();
	break;
	case 'h':
	HangupCall();
	break;
	case 'e':
	RedialCall();
	break;
	case 'i':
	ReceiveCall();
	break;
	}

	if (mySerial.available()>0)
	Serial.write(mySerial.read());
}

void SendMessage()
{
	Serial.println ("Sending sms"); 
  delay(1000);
  
  responce = ""; Latitude=""; Longitude=""; //initialise all string to null
  
  mySerial.println("AT+CIPGSMLOC=1,1"); 
  delay(5000); //Request for location data 

  while (mySerial.available()) {
  char letter = mySerial.read();
  responce = responce + String(letter); //Store the location information in string responce 
}

//use prepare_message funtion to prepare the link with the obtained LAT and LONG co-ordinates 
  prepare_message(); 
  delay(1000); 
  mySerial.println("AT+CMGF=1"); //Set the module in SMS mode
  delay(1000);

  mySerial.println("AT+CMGS=\"+918347152300\""); //Send SMS to this number 
  delay(1000);

  mySerial.println(Link); // we have send the string in variable Link 
  delay(1000);

  mySerial.println((char)26);// ASCII code of CTRL+Z - used to terminate the text message 
  delay(1000);

}
	
void ReceiveMessage()
{
	Serial.println("Received Message");
	mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
	delay(1000);
	if (mySerial.available()>0)
	{
	msg=mySerial.read();
	Serial.print(msg);
	}
}

void MakeCall()
{
	mySerial.println("ATD+918347152300;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
	Serial.println("Calling "); // print response over serial port
	delay(5000);
}

void HangupCall()
{
	mySerial.println("ATH");
	Serial.println("Hangup Call");
	delay(1000);
}

void ReceiveCall()
{
	mySerial.println("ATA");
	Serial.println("Received Call");
	delay(1000);
	{
	call=mySerial.read();
	Serial.print(call);
	}
}

void RedialCall()
{
	mySerial.println("ATDL");
	Serial.println("Redialing");
	delay(1000);
}

void prepare_message()
{
//Sample Output for AT+CIPGSMLOC=1,1   ==> +CIPGSMLOC: 0,75.802460,26.848892,2019/04/23,08:32:35 //where 26.8488832 is Lattitude and 75.802460 is longitute 
int first_comma = responce.indexOf(','); //Find the position of 1st comma 
int second_comma = responce.indexOf(',', first_comma+1); //Find the position of 2nd comma 
int third_comma = responce.indexOf(',', second_comma+1); //Find the position of 3rd comma 

for(int i=first_comma+1; i<second_comma; i++) //Values form 1st comma to 2nd comma is Longitude 
    Longitude = Longitude + responce.charAt(i); 

for(int i=second_comma+1; i<third_comma; i++) //Values form 2nd comma to 3rd comma is Latitude 
    Latitude = Latitude + responce.charAt(i); 

    Serial.println(Latitude); 
    Serial.println(Longitude); 
    
    Link = Link + Latitude + "," + Longitude; //Update the Link with latitude and Logitude values 
    Serial.println(Link);
}
