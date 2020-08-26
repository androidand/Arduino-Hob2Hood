#include <IRremote.h>

/*
This is a simplified version of https://github.com/tuxedo0801/Arduino-Hob2Hood
This version controls a central ventilation hood without a fan, and instead controls a motor to open or close the ventilation for a central ventilation hood.
Thus, the difference is that all "speed steps 1 to 4 below equals ventilation open.
Please note that Electrolux and AEG hobs and hoods use the same IR protocol since it's actually Electrolux making both brands.
This project controls a modified Franke Tender hood using an old Arduino Uno with an 1838 IR receiver and an Electrolux HOB750MF induction hob with Hob2Hood IR transmitter.

Reference original project (in german): http://knx-user-forum.de/forum/%C3%B6ffentlicher-bereich/knx-eib-forum/diy-do-it-yourself/37790-hob2hood-ir-protokoll-arduino/page2
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
const long IRCMD_LIGHT_ON = 0xE208293C;
const long IRCMD_LIGHT_OFF = 0x24ACF947;

// Pins for input switches/controls for manual control of the hood
const int PIN_IN_VENT = A0;
const int PIN_IN_LIGHT = A4;

// Pins for the output which controls a relay for the ventilation motor
const int PIN_OUT_VENT = 2;

const int PIN_OUT_LIGHT = 6;

// IR Receiver PIN
const int PIN_IR_RECEIVER = 11;

const int MODE_HOB2HOOD = 0;
const int MODE_MANUAL = 1;

// ventilation, light and mode status
int ventilation = 0;
int last_ventilation = 0;
int light = 0;
int last_light = 0;
int mode = 0; // 0 = hob2hood control, 1 = manual control

IRrecv irrecv(PIN_IR_RECEIVER); // create instance of 'irrecv'
decode_results results;

#define OFF HIGH
#define ON LOW

void setup() {

  // Init
  pinMode(PIN_OUT_VENT, OUTPUT);
  digitalWrite(PIN_OUT_VENT, OFF);

  pinMode(PIN_OUT_LIGHT, OUTPUT);
  digitalWrite(PIN_OUT_LIGHT, OFF);

  Serial.begin(9600); // for serial monitor output
  Serial.println("Hob2Hood Starting ...");

  Serial.println(" ... Setup IR receiver");
  irrecv.enableIRIn(); // Start the IR receiver
  Serial.println("Hob2Hood ready ...");
}

void loop() {

  // read manual control inputs
  int inLight = analogRead(PIN_IN_LIGHT);
  int inVentilation = analogRead(PIN_IN_VENT);

  // if any of the manual control inputs is IN USE (analog value >512) --> manual mode
  if (inLight >= 512 ||
      inVentilation >= 512 ) { // borde vända på >= till <= och sänka värdet om det är 3,2V till -1,9V...

    if (mode == MODE_HOB2HOOD) {
      Serial.println("Switching to manual mode");
    }

    mode = MODE_MANUAL;

    if (inLight > 512) {
      // Switch on the light:
      last_light = light;
      light = 1;
    } else {
      // Switch off the light:
      last_light = light;
      light = 0;
    }

    if (inVentilation1 > 512) {
      // set ventilation speed 1
      // set ventilation open
      last_ventilation = ventilation;
      ventilation = 1;
    } else if (inVentilation2 > 512) {
      // set ventilation speed 2
      // set ventilation open
      last_ventilation = ventilation;
      ventilation = 2;
    } else if (inVentilation3 > 512) {
      // set ventilation speed 3
      // set ventilation open
      last_ventilation = ventilation;
      ventilation = 3;
    } else if (inVentilation4 > 512) {
      // set ventilation speed 4
      // set ventilation open
      last_ventilation = ventilation;
      ventilation = 4;
    } else {
      // set ventilation off
      last_ventilation = ventilation;
      ventilation = 0;
    }

    controlHood();

  } else {

    // now we are in HOB2HOOD-mode, because no manual control is in use

    // check for previous mode
    if (mode == MODE_MANUAL) {
      Serial.println("Switching to Hob2Hood mode");
      // set to initial state
      ventilation = 0;
      light = 0;
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
        light = 1;
        break;

      case IRCMD_LIGHT_OFF:
        light = 0;
        break;

      case IRCMD_VENT_1:
        ventilation = 1;
        break;

      case IRCMD_VENT_2:
        ventilation = 2;
        break;

      case IRCMD_VENT_3:
        ventilation = 3;
        break;

      case IRCMD_VENT_4:
        ventilation = 4;
        break;

      case IRCMD_VENT_OFF:
        ventilation = 0;
        break;

      default:
        break;
    }

    controlHood();
    irrecv.resume(); // receive the next value
  }
}

// control hood based on 'light' and 'ventilation' variables
void controlHood() {

  bool logLight = light!=last_light;
  bool logVent = ventilation!=last_ventilation;
  

  // control light
  switch (light) {
    // Light OFF
    case 0:
      if (logLight) Serial.println("Light: OFF");
      digitalWrite(PIN_OUT_LIGHT, OFF);
      delay(10);
      break;
    // Light ON
    case 1:
      if (logLight) Serial.println("Light: ON");
      digitalWrite(PIN_OUT_LIGHT, ON);
      delay(10);
      break;
    default:
      break;
  }

  // control ventilation
  switch (ventilation) {

    // Ventilation OFF
    case 0:
      if (logVent) Serial.println("Ventilation: OFF");
      //digitalWrite(PIN_OUT_VENT_1, OFF);
      //digitalWrite(PIN_OUT_VENT_2, OFF);
      //digitalWrite(PIN_OUT_VENT_3, OFF);
      //digitalWrite(PIN_OUT_VENT_4, OFF);
      digitalWrite(PIN_OUT_VENT, OFF);
      delay(10);
      break;

    // Ventilation ON
    case 1:
      if (logVent) Serial.println("Ventilation: ON");
      //digitalWrite(PIN_OUT_VENT_2, OFF);
      //digitalWrite(PIN_OUT_VENT_3, OFF);
      //digitalWrite(PIN_OUT_VENT_4, OFF);
      //delay(100);
      digitalWrite(PIN_OUT_VENT, ON);
      delay(10);

      //delay(10);
      break;

//    // Ventilation Speed 2
//    case 2:
//      if (logVent) Serial.println("Ventilation: 2");
//      digitalWrite(PIN_OUT_VENT_1, OFF);
//      digitalWrite(PIN_OUT_VENT_3, OFF);
//      digitalWrite(PIN_OUT_VENT_4, OFF);
//      delay(100);
//      digitalWrite(PIN_OUT_VENT_2, ON);
//      delay(10);
//      break;
//
//    // Ventilation Speed 3
//    case 3:
//      if (logVent) Serial.println("Ventilation: 3");
//      digitalWrite(PIN_OUT_VENT_1, OFF);
//      digitalWrite(PIN_OUT_VENT_2, OFF);
//      digitalWrite(PIN_OUT_VENT_4, OFF);
//      delay(100);
//      digitalWrite(PIN_OUT_VENT_3, ON);
//      delay(10);
//      break;
//
//    // Ventilation Speed 4
//    case 4:
//      if (logVent) Serial.println("Ventilation: 4");
//      digitalWrite(PIN_OUT_VENT_1, OFF);
//      digitalWrite(PIN_OUT_VENT_2, OFF);
//      digitalWrite(PIN_OUT_VENT_3, OFF);
//      delay(100);
//      digitalWrite(PIN_OUT_VENT_4, ON);
//      delay(10);
//      break;

    default:
      break;

  }
  
  last_light = light;
  last_ventilation = ventilation;
  
}
