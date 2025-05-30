// LM35 is connected to this PIN
#define LM35 A1 

void setup() {
  // put your setup code here, to run once:
  // Init serial at 9600 baud
  Serial.begin(9600);

}

void loop() {

 //Read Raw ADC Data

  int adcData = analogRead(LM35);

  // Convert that ADC Data into voltage

  float voltage = adcData * (5.0 / 1024.0);

  // Convert the voltage into temperature

  float temperature = voltage * 100;

  // Print the temperature data

  Serial.print("Temperature: ");

  Serial.print(temperature);

  Serial.println("*C");

  delay(800); // wait a second between readings
}
