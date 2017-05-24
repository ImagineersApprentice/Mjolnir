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


SoftwareSerial xbeeSerial(4, 5); // Arduino RX, TX (XBee Dout, Din)

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
//Create reusable response objects for responses we expect to handle
Rx16Response rx16 = Rx16Response();

//data from capacitance
uint8_t data = 0;
//rssi is signal strength for proximity of gauntlet to mjolnir
uint8_t rssi = 0;

uint8_t d0Cmd[] = { 'D', '0' };
uint8_t d2Cmd[] = { 'D', '2' };
uint8_t d4Cmd[] = { 'D', '4' };
uint8_t dLow[] = { 0x4 };
uint8_t dHigh[] = { 0x5 };


XBeeAddress64 remoteAddress = XBeeAddress64(0x0013a200, 0x40a16e8d);
RemoteAtCommandRequest remoteAtRequestD0 = RemoteAtCommandRequest(remoteAddress, d0Cmd, dLow, sizeof(dLow));
RemoteAtCommandRequest remoteAtRequestD2 = RemoteAtCommandRequest(remoteAddress, d2Cmd, dLow, sizeof(dLow));
RemoteAtCommandRequest remoteAtRequestD4 = RemoteAtCommandRequest(remoteAddress, d4Cmd, dLow, sizeof(dLow));

int currentState = 1;       // Current state of lights

/***************************/
/***** Switch Setup ********/
/***************************/
const int switchWait = 2;
const int switchMwM = 3;


void setup() {

  // initalize NeoPixel
  lStrip.begin();
  tStrip.begin();
  lStrip.show(); // Initialize all pixels to 'off'
  tStrip.show(); // Initialize all pixels to 'off'

  // initialize the sensor pin as an input & enable the pullups
  pinMode(sensorPin, INPUT_PULLUP);

  // initialize Serial for XBee communication
  Serial.begin(9600);
  xbeeSerial.begin(57600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Start XBee");

  //initalize Switch
  pinMode(switchWait, INPUT_PULLUP);
  pinMode(switchMwM, INPUT_PULLUP);
  digitalWrite(switchMwM, HIGH);
  digitalWrite(switchWait, HIGH);

  remoteAtRequestD0.setCommand(d0Cmd);
  remoteAtRequestD0.setCommandValue(dLow);
  remoteAtRequestD0.setCommandValueLength(sizeof(dLow));
  xbee.send(remoteAtRequestD0);

  remoteAtRequestD2.setCommand(d2Cmd);
  remoteAtRequestD2.setCommandValue(dLow);
  remoteAtRequestD2.setCommandValueLength(sizeof(dLow));
  xbee.send(remoteAtRequestD2);

  remoteAtRequestD4.setCommand(d4Cmd);
  remoteAtRequestD4.setCommandValue(dLow);
  remoteAtRequestD4.setCommandValueLength(sizeof(dLow));
  xbee.send(remoteAtRequestD4);
}

void loop() {

  cd77colorallfill(20);
  Narcoleptic.delay(500); // During this time power consumption is minimised


  while (digitalRead(switchMwM) == LOW || digitalRead(switchWait) == LOW) {
    xbee.readPacket();

    // check XBee on Gauntlet
    if (xbee.getResponse().isAvailable()) {
      // got something
      // create a response
      xbee.getResponse().getRx16Response(rx16);
      data = rx16.getData(0);
      rssi = rx16.getRssi();
      //        Serial.print("RSSI: ");
      //        Serial.println(rssi);
      //        Serial.print("Data: ");
      //        Serial.println(char(data));

      if (digitalRead(switchWait) == LOW) {

          switch (data) {
            case '1':
              //Serial.println("Case 1");
              fadeIn(10);
              rainbow(10);
              fadeOut(10);
              return;
            case '2':
              //Mjolnir Gets Angry
              // lightning effect
              delay(50);
              angryLightning(0.08);
              delay(50);
              angryLightning(0.175);
              delay(50);
            case '3':
              //Serial.println("Case 3");
              if ( currentState != 0 ) {
                cd77colorallfillOn(200);
                currentState = 0;
              }
              else {
                cd77colorallfill(20);
                currentState = 1;
                delay(200);
              }
              break;
          }

        if (rssi >= 75) {
          //Mjolnir Gets Angry
          // lightning effect
          delay(50);
          angryLightning(0.08);
          delay(50);
          angryLightning(0.175);
          delay(50);
        }
        if (digitalRead(sensorPin) == LOW) {
          cd77colorallfillOn(200);
          delay(25);
          cd77colorallfill(200);
        }
      }
      if (digitalRead(switchMwM) == LOW) {
        switch (data) {
          case '1':
            //Serial.println("Case 1");
            remoteAtRequestD2.setCommand(d2Cmd);
            remoteAtRequestD2.setCommandValue(dHigh);
            remoteAtRequestD2.setCommandValueLength(sizeof(dHigh));
            xbee.send(remoteAtRequestD2);
            delay(20);
            remoteAtRequestD2.setCommand(d2Cmd);
            remoteAtRequestD2.setCommandValue(dLow);
            remoteAtRequestD2.setCommandValueLength(sizeof(dLow));
            xbee.send(remoteAtRequestD2);
            break;
          case '2':
            //Serial.println("Case 2");
            remoteAtRequestD0.setCommand(d0Cmd);
            remoteAtRequestD0.setCommandValue(dHigh);
            remoteAtRequestD0.setCommandValueLength(sizeof(dHigh));
            xbee.send(remoteAtRequestD0);
            delay(20);
            remoteAtRequestD0.setCommand(d0Cmd);
            remoteAtRequestD0.setCommandValue(dLow);
            remoteAtRequestD0.setCommandValueLength(sizeof(dLow));
            xbee.send(remoteAtRequestD0);
            break;
          case '3':
            //Serial.println("Case 3");
            if ( currentState != 0 ) {
              remoteAtRequestD4.setCommand(d4Cmd);
              remoteAtRequestD4.setCommandValue(dHigh);
              remoteAtRequestD4.setCommandValueLength(sizeof(dHigh));
              xbee.send(remoteAtRequestD4);
              delay(200);
              currentState = 0;
            }
            else {
              remoteAtRequestD4.setCommand(d4Cmd);
              remoteAtRequestD4.setCommandValue(dLow);
              remoteAtRequestD4.setCommandValueLength(sizeof(dLow));
              xbee.send(remoteAtRequestD4);
              currentState = 1;
              delay(200);
            }
          }
        }
        data = 0;
      }
  }
}


void angryLightning(float SpeedFactor) {

  /* / Make the lights breathe
    for (int i = 0; i < 100; i++) {
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactor * i));
    lStrip.setBrightness(intensity);
    tStrip.setBrightness(intensity);
    // Now set every LED to that color
    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {
    lStrip.setPixelColor(ledNumber, 250, 0, 0);
    tStrip.setPixelColor(ledNumber, 250, 0, 0);
    }

    lStrip.show();
    tStrip.show();
    //Wait a bit before continuing to breathe
    //delay(StepDelay);


    } */
  cd77colorallfillOnRed();
  delay(100);
  cd77colorallfill(20);
  delay(100);
  cd77colorallfillOnRed();
  delay(50);
  cd77colorallfill(20);
  delay(100);
  cd77colorallfillOnRed();
  delay(500);
  cd77colorallfill(20);
  delay(100);
  cd77colorallfillOnRed();
  delay(200);
  cd77colorallfill(20);

}
// Fills the NeoMatrix 8X8 panel with all of the NeoPixes at one time
void cd77colorallfillOnRed() {
  for (uint16_t i = 0; i < lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(255, 0, 0));
    tStrip.setPixelColor(i, tStrip.Color(255, 0, 0));
  }
  lStrip.show();
  tStrip.show();

}

// Fills the NeoMatrix 8X8 panel with all of the NeoPixes at one time
void cd77colorallfillOn(uint16_t wait) {
  for (uint16_t i = 0; i < lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(88, 195, 185));
    tStrip.setPixelColor(i, tStrip.Color(88, 195, 185));
  }
  lStrip.show();
  tStrip.show();
  delay(wait);
}

void cd77colorallfill(uint16_t wait) {
  for (uint16_t i = 0; i < lStrip.numPixels(); i++) {
    lStrip.setPixelColor(i, lStrip.Color(0, 0, 0));
    tStrip.setPixelColor(i, tStrip.Color(0, 0, 0));
  }
  lStrip.show();
  tStrip.show();
  delay(wait);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 185 - WheelPos;
  if (WheelPos < 85) {
    return lStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if (WheelPos < 195) {
    WheelPos -= 85;
    return lStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else {
    WheelPos -= 195;
    return lStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < lStrip.numPixels(); i++) {
      lStrip.setPixelColor(i, Wheel((i + j) & 255));
      tStrip.setPixelColor(i, Wheel((i + j) & 255));
    }
    lStrip.show();
    tStrip.show();
    delay(wait);
  }
}

void fadeIn(int waitT)
{
  int R = 0;
  int G = 0;
  int B = 0;
  int finCount = 30;
  int Rset = 125;
  int Gset = 125;
  int Bset = 125;

  //Fade in
  while (1) { //using an inf loop to be more custom.

    //break the inf loop if the color is higher then what its set at.
    if (R > Rset + 1 && G > Gset + 1 && B > Bset + 1)  {
      //ReSet the RGB to set values.
      R = Rset;
      G = Gset;
      B = Bset;
      break;
    }
    //update the strip
    for (int i = 0; i < lStrip.numPixels(); i++) {
      lStrip.setPixelColor(i, lStrip.Color(R, G, B));
      tStrip.setPixelColor(i, tStrip.Color(R, G, B));
      lStrip.show();
      tStrip.show();
      delay(0);
    }
    //increase by the set amount
    R = R + finCount;
    G = G + finCount;
    B = B + finCount;
    delay(waitT);
  }
}

void fadeOut(int waitT) {

  int R = 0;
  int G = 0;
  int B = 0;
  int foutCount = 30;
  while (1) { //using an inf loop to be more custom.
    //Protect the strand from higher then 255 values
    if (R > 255 || G > 255 || B > 255) {
      break;
    } //DO NOT DELETE OR ALTER THIS LINE.
    //break the inf loop if the color is off
    if (R < 0 && G < 0 && B < 0)  {
      //ReSet the RGB to 0 values.
      R = 0;
      G = 0;
      B = 0;
      break;
    }
    //update the strip
    for (int j = 0; j < lStrip.numPixels(); j++) {
      lStrip.setPixelColor(j, lStrip.Color(R, G, B));
      tStrip.setPixelColor(j, lStrip.Color(R, G, B));
      lStrip.show();
      tStrip.show();
      delay(0);
    }
    //Decrease by the set amount
    R = R - foutCount;
    G = G - foutCount;
    B = B - foutCount;
    delay(waitT);
  }
}
