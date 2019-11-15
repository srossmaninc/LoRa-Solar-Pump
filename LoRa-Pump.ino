#include <Wire.h>
#include <Adafruit_GFX.h>

#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>

// OLED shit
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

int waterValue = 850;

SoftwareSerial mDot(2, 3); //RX & TX

void setup() {
  Serial.begin(9600);
  mDot.begin(9600);
  mDot.println("AT+JOIN");
  
  pinMode(relay_pin, OUTPUT);
  
  pinMode(moisturePower, OUTPUT);
  digitalWrite(moisturePower, LOW);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
}
  delay(2000);

}

int read_water_sensor() {
  digitalWrite(moisturePower, HIGH);
  delay(500);
  int value = analogRead(moistureAnalog);
  digitalWrite(moisturePower, LOW);
  return 1023 - value;
}

void loop() {
  int water_value = read_water_sensor();
  
  // Serial Moniter and pump relay control
  Serial.print("Water Level (0-1023): ");
  Serial.println(water_value);
  if (water_value < waterValue) {
    digitalWrite(relay_pin, HIGH);
    Serial.println("Relay On");
  } else {
    digitalWrite(relay_pin, LOW);
    Serial.println("Relay Off");
  }

  // OLED Display
  display.clearDisplay();
  delay(100);
  display.setRotation(1);
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  // SM stands for Soil Moisture
  display.println("SM:");
  //display.setCursor(0,3);
  display.println(water_value);
  display.display();

  // LoRa Stuff
  String packet = "AT+SEND=";
  packet += water_value;
  Serial.println(packet);
  mDot.println(packet);
  delay(30000);
}
