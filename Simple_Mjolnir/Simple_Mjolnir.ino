#include <Narcoleptic.h>
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

const int sensorPin =  7;      // the number of the sensor pin

/********************************/
/***** XBee Sensor Setup ********/
/********************************/


SoftwareSerial xbeeSerial(2, 3); // Arduino RX, TX (XBee Dout, Din)

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
  digitalWrite(switchOn, HIGH);
  digitalWrite(switchWait, HIGH);
}

void loop() {

  Narcoleptic.delay(500); // During this time power consumption is minimised


  while (digitalRead(switchOn) == LOW || digitalRead(switchWait) == LOW){

    if (digitalRead(switchWait) == LOW && digitalRead(switchOn) == HIGH){
      xbee.readPacket();

      // check XBee on Gauntlet
      if (xbee.getResponse().isAvailable()) {
        // got something

        // create a response
        xbee.getResponse().getRx16Response(rx16);
        data = rx16.getData(0);
        rssi = rx16.getRssi();
        //Serial.print("RSSI: ");
        //Serial.print(rssi);
        //Serial.println();
        //Serial.print("Data: ");
        //Serial.print(data);
        //Serial.println();        


        // button is pressed
        // pretty lights

        if (data == 49){

          fade();


        }

        // if Mjolnir is too far from the gauntlet 
        //get angry
        if (rssi >=80){

          //Mjolnir Gets Angry
          // lightning effect
          angryLightning();

        }

      }
      // check the Vibration Sensor  
      if (digitalRead(sensorPin) == LOW){

        cd77colorallfillOn();
        delay(25);
        cd77colorallfill();


      }
    }

    if (digitalRead(switchOn) == LOW && digitalRead(switchWait == HIGH)){
      cd77colorallfillOn();
    }

  }

  cd77colorallfill();


}

// Fills the NeoMatrix 8X8 panel with all of the NeoPixes at one time 
void cd77colorallfillOn() {  
  for(uint16_t i=0; i<lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(88, 195, 185));   
    tStrip.setPixelColor(i, tStrip.Color(88, 195, 185));   
  }       
  lStrip.show();
  tStrip.show();

}

// Fills the NeoMatrix 8X8 panel with all of the NeoPixes at one time 
void cd77colorallfillOnRed() {  
  for(uint16_t i=0; i<lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(255, 0, 0));   
    tStrip.setPixelColor(i, tStrip.Color(255, 0, 0));   
  }       
  lStrip.show();
  tStrip.show();

}

void cd77colorallfill() {  
  for(uint16_t i=0; i<lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(0, 0, 0));   
    tStrip.setPixelColor(i, tStrip.Color(0, 0, 0));   
  }       
  lStrip.show();
  tStrip.show();
}



void fade()
{
  int Red= 0;
  int Green = 0;
  int Blue = 0;
  int finCount=5;
  int foutCount=5;
  int redSet = 125;
  int greenSet = 125;
  int blueSet = 125;
  int waitT = 5;
  //Fade in
  while(1){ //using an inf loop to be more custom.
    //Protect the strand from higher then 255 values
    if(Red>255 || Green>255 || Blue>255) { 
      break; 
    } //DO NOT DELETE OR ALTER THIS LINE.
    //break the inf loop if the color is higher then what its set at.
    if (Red>redSet+1 && Green>greenSet+1 && Blue>blueSet+1)  { 
      //ReSet the RGB to set values. 
      Red=redSet;
      Green=greenSet;
      Blue=blueSet;
      break; 
    } 
    //update the strip
    for(int i=0; i<tStrip.numPixels(); i++) {
      tStrip.setPixelColor(i, tStrip.Color(Red, Green, Blue));
      lStrip.setPixelColor(i, tStrip.Color(Red, Green, Blue));
      tStrip.show();
      lStrip.show();
      delay(0);
    }
    //increase by the set amount
    Red=Red+finCount;
    Green=Green+finCount;
    Blue=Blue+finCount;
    delay(waitT);
  }
  //Fade Out
  while(1){ //using an inf loop to be more custom.
    //Protect the strand from higher then 255 values
    if(Red>255 || Green>255 || Blue>255) { 
      break; 
    } //DO NOT DELETE OR ALTER THIS LINE.
    //break the inf loop if the color is off
    if (Red<0 && Green<0 && Blue<0)  { 
      //ReSet the RGB to 0 values. 
      Red=0;
      Green=0;
      Blue=0;
      break; 
    } 
    //update the strip
    for(int j=0; j<tStrip.numPixels(); j++) {
      tStrip.setPixelColor(j, tStrip.Color(Red, Green, Blue));
      lStrip.setPixelColor(j, tStrip.Color(Red, Green, Blue));
      tStrip.show();
      lStrip.show();
      delay(0);
    }
    //Decrease by the set amount
    Red=Red-foutCount;
    Green=Green-foutCount;
    Blue=Blue-foutCount;
    delay(waitT);
  }
}

void angryLightning()
{
  cd77colorallfillOnRed();
  delay(100);
  cd77colorallfill();
  delay(100);
  cd77colorallfillOnRed();
  delay(50);
  cd77colorallfill();
  delay(100);
  cd77colorallfillOnRed();
  delay(500);
  cd77colorallfill();
  delay(100);
  cd77colorallfillOnRed();
  delay(200);
  cd77colorallfill();
}





