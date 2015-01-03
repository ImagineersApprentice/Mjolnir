
/*
 * A simple example program that will send text every second.
 * I use this to make sure my Fio and XBee are working appropriately.
 *
 * I hook another XBee up to my computer with an breakout board and
 * use CoolTerm to display the messages.
 */
 
  /*********************
    **** Switch Setup ****
    *********************/
    const int buttonPin = 2;     // the number of the pushbutton pin
    const int ledPin =  13;      // the number of the LED pin    
    int buttonState = 0;         // variable for reading the pushbutton status
    int lastButtonState = 0;
    int ledState = 0;
    boolean onOff = false;
    boolean lastOnOff = false;

void setup() {
  Serial.begin(57600);	// opens port at 57600 bps
    // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);   

  flashLed(ledPin, 2, 50);
}

void loop() {

  buttonState = digitalRead(buttonPin);
  ledState = digitalRead(ledPin);
  

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState != lastButtonState){

  Serial.print(!onOff);
  Serial.write(10); //
  digitalWrite(ledPin, !ledState);
  
  
  }
  else{
  Serial.print(lastOnOff);
  Serial.write(10);
  }
  
  lastButtonState = buttonState;
  lastOnOff = onOff;
  
    // wait 1 second then loop again
  delay(1000);
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

