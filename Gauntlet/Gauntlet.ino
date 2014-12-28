

    /*******************
    **** XBee Setup ****
    *******************/
    
    #include <XBee.h>
    
    XBee xbee = XBee();
    
    unsigned long start = millis();
   
  
    //Create reusable response objects for responses we expect to handle
    Rx16Response rx16 = Rx16Response();
    
    // allocate two bytes for to hold a 10-bit analog reading
    
    TxStatusResponse txStatus = TxStatusResponse();
    

    /****************************
    **** Muscle Sensor Setup ****
    ****************************/
    
    int sensorPin = A7;    // select the input pin for the potentiometer
    int ledPin = 13;      // select the pin for the LED
    int sensorValue = 0;  // variable to store the value coming from the sensor
    

void setup() {
  //setup LED
  pinMode(ledPin, OUTPUT);

  //serial
  Serial.begin(57600);
  xbee.setSerial(Serial);

  flashLed(ledPin, 2, 50);
}


void loop() {

  // if muscle sensor is aabove 30, consider it "activated"
  sensorValue = analogRead(sensorPin);    
  if (sensorValue > 20){

    flashLed(ledPin, 4, 50);
    
    uint8_t payload[] = { 0, 1 };
    
    Tx16Request tx = Tx16Request(0x1874, payload, sizeof(payload));
    
    xbee.send(tx);
    
    flashLed(ledPin, 1, 10);

    
  }
  else {
    
    uint8_t payload[] = { 0, 0 };
    
    Tx16Request tx = Tx16Request(0x1874, payload, sizeof(payload));
    
    xbee.send(tx);
    
    flashLed(ledPin, 1, 10);


  
  }
  delay(100);
}


void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}


