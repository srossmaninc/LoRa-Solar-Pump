#include <SoftwareSerial.h>

/* This script essentially turns on the pump via a relay when the water is low enough
 *  We don't need to automate the aerator since it should always be on.
 *  
 *  This script uses the mDot as the LoRa Module
 */

// Application EUI: 70B3D57ED00108C6
// App Key: 623F753B86CC9AAAE228548FF454E9BF

int moistureAnalog = A0;
int moisturePower = 8; //5V will burn the sensor thingy so this turns on and off power instead
int relay_pin = 7;

int waterValue = "Needs to be set";

SoftwareSerial mDot(2, 3); //RX & TX

void setup() {
  Serial.begin(9600);
  mDot.begin(9600);
  mDot.println("AT+JOIN");
  
  pinMode(relay_pin, OUTPUT);
  
  pinMode(moisturePower, OUTPUT);
  digitalWrite(moisturePower, LOW);
}

int read_water_sensor() {
  digitalWrite(moisturePower, HIGH);
  delay(500);
  int value = analogRead(moistureAnalog);
  digitalWrite(moisturePower, LOW);
  return 1023 - value;
}

void loop() {
  Serial.print("Water Level (0-1023): ");
  Serial.println(read_water_sensor());
  if (read_water_sensor() == waterValue) {
    digitalWrite(relay_pin, HIGH);
  } else {
    digitalWrite(relay_pin, LOW);
  }
  String packet = "AT+SEND=";
  packet += read_water_sensor();
  mDot.println(packet);
  delay(60000);
}
