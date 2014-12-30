#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <XBee.h>

/*****************************/
/***** NeoPixel Setup ********/
/*****************************/

#define lPIN 6
#define tPIN 9

//Lightning Code
//Written by: Jason Yandell  
int TOTAL_LEDS = 64;
float MaximumBrightness = 255;
float StepDelay = 5; // ms for a step delay on the lights

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel lStrip = Adafruit_NeoPixel(TOTAL_LEDS, lPIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tStrip = Adafruit_NeoPixel(TOTAL_LEDS, tPIN, NEO_GRB + NEO_KHZ800);

/*************************************/
/***** Vibration Sensor Setup ********/
/*************************************/

const int sensorPin =  5;      // the number of the sensor pin

/********************************/
/***** XBee Sensor Setup ********/
/********************************/


SoftwareSerial xbeeSerial(4, 5); // Arduino RX, TX (XBee Dout, Din)

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
//Create reusable response objects for responses we expect to handle
Rx16Response rx16 = Rx16Response();

//data is activation from muscle sensor
uint8_t data = 0;
//rssi is signal strength for proximity of gauntlet to mjolnir
uint8_t rssi = 0;


/***************************/
/***** Switch Setup ********/
/***************************/
const int switchWait = 2;
const int switchOn = 3;
int switchWaitState = 0;
int switchOnState = 0;


void setup(){

  // initalize NeoPixel
  lStrip.begin();
  tStrip.begin();
  lStrip.show(); // Initialize all pixels to 'off'
  tStrip.show(); // Initialize all pixels to 'off'

  // initialize the sensor pin as an input & enable the pullups
  pinMode(sensorPin, INPUT_PULLUP);  

  // initialize Serial for XBee communication
  Serial.begin(57600);
  xbee.setSerial(Serial);

  //initalize Switch
  pinMode(switchWait, INPUT_PULLUP);
  pinMode(switchOn, INPUT_PULLUP);
}

void loop() {

  switchWaitState = digitalRead(switchWait);
  switchOnState = digitalRead(switchOn);

  if (switchWaitState == LOW){
    xbee.readPacket();

    // check XBee on Gauntlet
    if (xbee.getResponse().isAvailable()) {
      // got something

      // create a response
      xbee.getResponse().getRx16Response(rx16);
      data = rx16.getData(0);
      rssi = rx16.getRssi();
      Serial.print("RSSI: ");
      Serial.print(rssi);
      Serial.println();

      // if Mjolnir is too far from the gauntlet 
      //get angry
      if (rssi >=65){

        //Mjolnir Gets Angry
        // lightning effect
        delay(50);
        lightning(0.08);
        delay(50);
        lightning(0.175);
        delay(50);
        cd77colorallfill(200);

      }
      // if muscle is activated
      // pretty lights
      else if (data == 1){
      }
    }
    // check the Vibration Sensor  
    /*else if (digitalRead(sensorPin) == LOW){
     // lightning effect
     delay(50);
     lightning(0.08);
     delay(50);
     lightning(0.175);
     delay(50);
     cd77colorallfill(200);
     
     }*/
  }
  else if (switchOnState == LOW){

    delay(50);
    lightningR(0.08);
    delay(50);
    lightningR(0.175);
    delay(50);
    cd77colorallfill(200);    

  }


}

void lightning(float SpeedFactor){

  // Make the lights breathe
  for (int i = 0; i < 100; i++) {
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactor * i));
    lStrip.setBrightness(intensity);
    // Now set every LED to that color
    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {
      lStrip.setPixelColor(ledNumber, 88, 195, 185);
    }

    lStrip.show();
    //Wait a bit before continuing to breathe
    delay(StepDelay);
  } 
}

// Fills the NeoMatrix 8X8 panel with all of the NeoPixes at one time 
void cd77colorallfill(uint16_t wait) {  
  for(uint16_t i=0; i<lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(0, 0, 0));      
  }       
  lStrip.show();
  delay(wait);
}    

void lightningR(float SpeedFactor){

  // Make the lights breathe
  for (int i = 0; i < 100; i++) {
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactor * i));
    lStrip.setBrightness(intensity);
    // Now set every LED to that color
    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {
      lStrip.setPixelColor(ledNumber, 250, 0, 0);
    }

    lStrip.show();
    //Wait a bit before continuing to breathe
    delay(StepDelay);
  } 
}




