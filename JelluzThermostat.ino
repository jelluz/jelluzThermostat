/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <SPI.h>
#include <MySensor.h>
#include <String.h>
#include <stdio.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void setup();
void loop();
//End of Auto generated function prototypes by Atmel Studio

// this constant won't change:
const int U_D = 2;   // Up- / downmode for DS1804
const int Clk = 3;   // Clock for DS1804
const int CE = 4;    // chip select for DS1804
const int LED = 5;   // separate LED pin
const int S1 = 6;    // pushbutton 1
const int S2 = 7;    // pushbutton 2

// Variables will change:
int S1_cnt = 0;     // debounce counter
int S2_cnt = 0;     // debounce counter
int LED_state = 0;      // previous state of the button
char buffer[30] = " ";        // printbuffer
int wiper = 0;

#define CHILD_ID_TEMP 0
#define HUMIDITY_SENSOR_DIGITAL_PIN 3
unsigned long SLEEP_TIME = 1000; // Sleep time between reads (in milliseconds)

MySensor gw;
//DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
MyMessage msgTSetpoint(S_HVAC, V_TEMP);


void increaseWiper() {
  digitalWrite(U_D, HIGH);
  digitalWrite(CE, LOW);
  digitalWrite(Clk, HIGH);
  //     delay(1);
  wiper++;
  if (wiper > 100)
  wiper = 100;
  sprintf(buffer, "up : wiper = %i", wiper);
  Serial.println(buffer);
  digitalWrite(Clk, LOW);
  digitalWrite(CE, HIGH);
}

void decreaseWiper() {
  digitalWrite(U_D, LOW);
  digitalWrite(CE, LOW);
  digitalWrite(Clk, HIGH);
  //     delay(1);
  wiper--;
  if (wiper < 0)
  wiper = 0;
  sprintf(buffer, "down : wiper = %i", wiper);
  Serial.println(buffer);

  digitalWrite(Clk, LOW);
  digitalWrite(CE, HIGH);
}


int celsius2washer(float T)
{

  float offset_w = 10;
  float offset_t = 8.5;
  float end_w = 90;
  float end_t = 32.5;
  float dwdt = 0;
  int washer=0;
  
  dwdt = (end_w - offset_w)/(end_t - offset_t);
  
  washer = int((T-offset_t)*dwdt + offset_w);
  if (washer>100)
  washer = 100;
  if (washer <0)
  washer = 0;
  return washer;
}

void setTemp(byte value) {
  for (int i = 0; i < 100; i++) {
    decreaseWiper();
  }
  for (int i = 0; i < value; i++) {
    increaseWiper();
  }

}

void getVariables(const MyMessage &message)
{
  int washer = 0;
  
  Serial.print("message received :)");
  Serial.print(message.getFloat());
  washer = celsius2washer(message.getFloat());
  setTemp( washer);
  Serial.print(washer);

}




void checkButtons(){
  if (digitalRead(S1) == LOW) {
    if (S1_cnt < 100)
    S1_cnt++;
    if ((S1_cnt == 50) && (S2_cnt == 0)) {
      decreaseWiper();
      S1_cnt = 100;                 // add hysteresis
    }
  }
  if ((digitalRead(S1) == HIGH) && (S1_cnt > 0))
  S1_cnt--;

  if (digitalRead(S2) == LOW) {
    if (S2_cnt < 100)
    S2_cnt++;
    if ((S2_cnt == 50) && (S1_cnt == 0)) {
      increaseWiper();
      S2_cnt = 100;                       // add hysteresis
    }
  }
  if ((digitalRead(S2) == HIGH) && (S2_cnt > 0))
  S2_cnt--;
}



void setup()
{

  pinMode(CE, OUTPUT);
  pinMode(U_D, OUTPUT);
  pinMode(Clk, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  digitalWrite(CE, HIGH);        //DS1804 inactive
  digitalWrite(U_D, LOW);        //Count down
  digitalWrite(Clk, LOW); //Clk inactive
  digitalWrite(LED, HIGH);  //LED off
  digitalWrite(S1, HIGH);        //enable pull-up
  digitalWrite(S2, HIGH);        //enable pull-up

  // initialize serial communication:
  //Serial.begin(115200);   //pin 0 = input, pin 1 = output
  digitalWrite(U_D, LOW);
  digitalWrite(CE, LOW);
  setTemp(35);

  gw.begin(getVariables,7);
  //dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("Thermostat", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_TEMP, S_TEMP);
  
  metric = gw.getConfig().isMetric;
}

void loop()
{
  
  gw.process();
}


