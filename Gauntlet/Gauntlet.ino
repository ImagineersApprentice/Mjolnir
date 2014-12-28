#include <XBee.h>

XBee xbee = XBee();

unsigned long start = millis();

uint8_t payload[] = { 0, 0 };

Tx16Request tx = Tx16Request(0x1874, payload, sizeof(payload));

//Create reusable response objects for responses we expect to handle
Rx16Response rx16 = Rx16Response();

// allocate two bytes for to hold a 10-bit analog reading

TxStatusResponse txStatus = TxStatusResponse();

int statusLed=13;

void setup() {
  //setup LED
  pinMode(statusLed, OUTPUT);

  //serial
  Serial.begin(57600);
  xbee.setSerial(Serial);

  flashLed(statusLed, 3, 50);
}


void loop() {

    xbee.send(tx);
    
    flashLed(statusLed, 1, 10);

    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
      // got a response!

      // should be a znet tx status                   
      if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);

        // get the delivery status, the fifth byte
        if (txStatus.getStatus() == SUCCESS) {
          // success.  time to celebrate
          flashLed(statusLed, 5, 50);
        } 

      }      
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


