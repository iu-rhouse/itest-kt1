#include <Arduino.h>
#include "utilities.h"
#include "create_open_interface.h" // formerly COI.h
#include "experiment_oct_2013.h"
#include "analog_sensors.h"
#include "i2c_sensors.h"
#include "create_sensors.h"
#include "communication.h"
#include "movement.h"
#include "katie_autonomous.h"
#include "unit_testing.h"
#include "experiment_dec_2013.h" // exp_stream
#include "experiment_feb_2014.h"
void setup();
void loop();
#line 1 "src/sketch.ino"
//#include "utilities.h"
//#include "create_open_interface.h" // formerly COI.h
//#include "experiment_oct_2013.h"
//#include "analog_sensors.h"
//#include "i2c_sensors.h"
//#include "create_sensors.h"
//#include "communication.h"
//#include "movement.h"
//#include "katie_autonomous.h"
//#include "unit_testing.h"
//#include "experiment_dec_2013.h" // exp_stream
//#include "experiment_feb_2014.h"

#define CREATE_RX           7
#define CREATE_TX           4



void setup() {
  Serial.begin(115200);
  init_i2c_sensors();
  COIInitialize (CREATE_RX, CREATE_TX);
  // COIChangeMode(CMD_MODE_SAFE);
  Serial.println("Talking with Create");
  pinMode(INPUT_20, INPUT);  // indicates script on // finished.
  COISetDigitalOutput(2,LOW);
  get_katie_id();

}

int picture_timer = 0; /// start time for pictures

void loop() {

  recieve_commands();
  
  if (!docking && !auto_cover) {
    execute_commands();
    interaction();
  }
  
  if (stream)   // @t
    stream_data();

  if (exp_auto) // @a
    feb_experiment_scanning();//katie_auto();

  if (auto_on)   // @u
    katie_auto();

  // if (auto_cover){ /// send state message at a regular interval during script
  //   int diff = millis() - picture_timer;
  //   if (diff > 3000) {
  //     send_state("picture timer");
  //     picture_timer = millis();
  //   }
  // }

  if (exp_stream)  // @e
    arduino_hub_stream(); // experiment_dec_2013

  if (debug_stream)   // @g
    stream_debug_sensors();

  if (!digitalRead(INPUT_20) && !auto_cover) { // check to see if not in script so command does not interfere
    update_create_fast_sensors(); // so that collision detection works
    charger();  // detects charger (once this works move make nest it deeper in framework)
  }

  delay(20); // serial gets overrun if no delay

}






