#include <IRremote.h>

/*
This is based on https://github.com/tuxedo0801/Arduino-Hob2Hood
This version controls a central ventilation hood without a fan, and instead controls a motor to open or close the ventilation for a central ventilation hood.
Please note that Electrolux and AEG hobs and hoods use the same IR protocol since it's actually Electrolux making both brands.
This project controls a modified Franke Tender hood using an old Arduino Uno with an 1838 IR receiver and an Electrolux HOB750MF induction hob with Hob2Hood IR transmitter.

Ventilation Step1 0xE3C01BE2
Ventilation Step2 0xD051C301
Ventilation Step3 0xC22FFFD7
Ventilation Step4 0xB9121B29
Ventilation Off 0x55303A3
Light On 0xE208293C
Light Off 0x24ACF947
*/

// IR commands from Electrolux/AEG hob2hood device
const long IRCMD_VENT_1 = 0xE3C01BE2;
const long IRCMD_VENT_2 = 0xD051C301;
const long IRCMD_VENT_3 = 0xC22FFFD7;
const long IRCMD_VENT_4 = 0xB9121B29;
const long IRCMD_VENT_OFF = 0x55303A3;

// IR commands from a remote, just for testing
const long IRCMD_TEST_ON = 0x80A;
const long IRCMD_TEST_OFF = 0x800;

const long IRCMD_LIGHT_ON = 0xE208293C;
const long IRCMD_LIGHT_OFF = 0x24ACF947;

// Pins for input switches/controls for manual control of the hood
const int PIN_IN_VENT = 8; //A0;
const int PIN_IN_LIGHT = 9; //A4;

// IR Receiver PIN
const int PIN_IR_RECEIVER = 11;

// Pins for the output which controls a relay for the ventilation motor
const int PIN_OUT_VENT = 2;
const int PIN_OUT_LIGHT = 4; 

const int PIN_OUT_LED = 13; 

const int MODE_HOB2HOOD = 0;
const int MODE_MANUAL = 1;

// ventilation, light and mode states
int ventButtonInput = LOW; //0
int ventButtonState = LOW;
int ventButtonPrevious = LOW;  

int ventOutput = LOW;


int lightButtonInput = LOW;
int lightButtonState = LOW;
int lightButtonPrevious = LOW;

int lightOutput = LOW;

int mode = 0; // 0 = hob2hood control, 1 = manual control


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

IRrecv irrecv(PIN_IR_RECEIVER); // create instance of 'irrecv'
decode_results results;

void setup() {

  // Init

  pinMode(PIN_IN_VENT, INPUT);
  pinMode(PIN_IN_LIGHT, INPUT);
  pinMode(PIN_IR_RECEIVER, INPUT);
  
  pinMode(PIN_OUT_VENT, OUTPUT);
  digitalWrite(PIN_OUT_VENT, LOW);

  pinMode(PIN_OUT_LIGHT, OUTPUT);
  digitalWrite(PIN_OUT_LIGHT, LOW);

  Serial.begin(9600); // for serial monitor output
  Serial.println("Hob2Hood Starting ...");

  Serial.println(" ... Setup IR receiver");
  irrecv.enableIRIn(); // Start the IR receiver
  Serial.println("Hob2Hood ready ...");
}

void loop() {

  // read manual control inputs
  
  ventButtonInput = digitalRead(PIN_IN_VENT);
  lightButtonInput = digitalRead(PIN_IN_LIGHT);

  // if any of the manual control inputs is IN USE --> manual mode
  if (lightButtonInput == HIGH ||
      ventButtonInput == HIGH ) {

    if (mode == MODE_HOB2HOOD) {
      Serial.println("Switching to manual mode");
    }

    mode = MODE_MANUAL;


    if (ventButtonInput == HIGH)
    {Serial.println(ventButtonInput);}

    if (lightButtonInput == HIGH)
    {Serial.println(lightButtonInput);}



  // If the switch changed, due to noise or pressing:
  if ((ventButtonInput != ventButtonPrevious) || (lightButtonInput != lightButtonPrevious)) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  

// VENT
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (ventButtonInput != ventButtonState) {
      ventButtonState = ventButtonInput;

      // only toggle the LED if the new button state is HIGH
      if (ventButtonState == HIGH) {
        ventOutput = !ventOutput;
      }
    }
  }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  ventButtonPrevious = ventButtonInput;

 //


// LIGHT
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (lightButtonInput != lightButtonState) {
      lightButtonState = lightButtonInput;

      // only toggle the LED if the new button state is HIGH
      if (lightButtonState == HIGH) {
        lightOutput = !lightOutput;
      }
    }
  }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lightButtonPrevious = lightButtonInput;



 
 // 

    controlHood();



  } else {

    // now we are in HOB2HOOD-mode, because no manual control is in use
    // This doesn't work does it. As soon as button is released...

    // check for previous mode
    if (mode == MODE_MANUAL) {
      Serial.println("Switching to Hob2Hood mode");
      // Well, we can't just set to initial state
       ventButtonState = LOW;
       lightButtonState = LOW;
       controlHood();

      // and switch to hob2hood mode
      mode = MODE_HOB2HOOD;
      irrecv.resume();
    }
    receiveIRCommand();
  }

}

// Receive and decode IR commands and control hood upon received command
void receiveIRCommand() {

  // have we received an IR signal?
  if (irrecv.decode(&results)) {

    Serial.println("Received IR command: ");
    Serial.println(results.value, HEX); // display it on serial monitor in hexadecimal

    switch (results.value) {

      case IRCMD_LIGHT_ON:
        lightOutput = HIGH;
        break;

      case IRCMD_LIGHT_OFF:
        lightOutput = LOW;
        break;

      case IRCMD_VENT_1:
        ventOutput = HIGH;
        break;

      case IRCMD_VENT_2:
        ventOutput = HIGH;
        break;

      case IRCMD_VENT_3:
        ventOutput = HIGH;
        break;

      case IRCMD_VENT_4:
        ventOutput = HIGH;
        break;

      case IRCMD_VENT_OFF:
        ventOutput = LOW;
        break;

      case IRCMD_TEST_ON:
        lightOutput = HIGH;
        break;

      case IRCMD_TEST_OFF:
        lightOutput = LOW;
        break;

      default:
      Serial.println("Unknown IR command");
        break;
    }

    controlHood();
    irrecv.resume(); // receive the next value
  }
}

// control hood based on 'light' and 'vent' variables
void controlHood() {

  //bool logLight = lightOutput!=lightButtonPrevious;
  //bool logVent = ventOutput!=ventButtonPrevious;
  

  // control light
  switch (lightOutput) {
    // Light OFF
    case LOW:
      //if (logLight) 
      Serial.println("Light: OFF");
      digitalWrite(PIN_OUT_LIGHT, LOW);
      digitalWrite(PIN_OUT_LED, LOW);
      delay(10);
      break;
    // Light ON
    case HIGH:
      //if (logLight) 
      Serial.println("Light: ON");
      digitalWrite(PIN_OUT_LIGHT, HIGH);
      digitalWrite(PIN_OUT_LED, HIGH);
      delay(10);
      break;
    default:
      break;
  }

  // control ventilation
  switch (ventOutput) {

    // Ventilation OFF
    case LOW:
      //if (logVent) 
      Serial.println("Ventilation: OFF");
      digitalWrite(PIN_OUT_VENT, LOW);
      
      delay(10);
      break;

    // Ventilation ON
    case HIGH:
      //if (logVent) 
      Serial.println("Ventilation: ON");
      digitalWrite(PIN_OUT_VENT, HIGH);
      
      delay(10);
      break;

    default:
      break;

  }
  
  //last_light = light;
  //last_vent = vent;
  
}
