#include "utilities.h"
#include "Servo.h" 
#include "create_open_interface.h" // formerly COI.h
#include "experiment_oct_2013.h"
#include "analog_sensors.h"
#include "i2c_sensors.h"
#include "create_sensors.h"
#include "timed_movements.h"
#include "communication.h"
#include "movement.h"

#include "katie_autonomous.h"
#include "unit_testing.h"
#include "experiment_dec_2013.h" // exp_stream
#include "experiment_feb_2014.h"

#define CREATE_RX           7
#define CREATE_TX           4




void setup() {
  Serial.begin(115200);
  init_i2c_sensors();
  COIInitialize (CREATE_RX, CREATE_TX);

  Serial.println("Talking with Create");
  pinMode(INPUT_20, INPUT);  // indicates script on // finished.
  COISetDigitalOutput(2,LOW);
  get_katie_id();
  
  camera_arm.attach(9,175,180);
  delay(200);
  camera_arm.detach();

}



int picture_timer = 0; /// start time for pictures
int heading = 0; // current direction the robot is facing
int bp = false;
void loop() {

  recieve_commands();
  
  if (!docking && !auto_cover) {
    execute_move();
    interaction();
  }
  
  if (move_time == 0) {

    if (stream)   // @t
      stream_data();

    if (exp_auto) // @a
      feb_experiment_scanning();//katie_auto();

    if (auto_on )   // @u
      katie_auto();

    // if (auto_cover){ /// send state message at a regular interval during script
    //   int diff = millis() - picture_timer;
    //   if (diff > 3000) {
    //     send_state("picture timer");
    //     picture_timer = millis();
    //   }
    // }

    if (exp_stream )  // @e
      // arduino_hub_stream(); // experiment_dec_2013

    if (debug_stream )   // @g
      stream_debug_sensors();
  }

  BUTTONS_STATE = COIFetchSingleSensor(BUTTONS);
  if (PLAY_BUTTON(BUTTONS_STATE) == HIGH) {
      delay(200);
      COIChangeMode(CMD_MODE_PASSIVE);
      play_toggle = true;
      send_response("SAFE MODE ON", 1);
    }

  if ((CHARGING_SOURCE_STATE == 1  || CHARGING_SOURCE_STATE == 2) && play_toggle == true ) {
    charge_toggle = true;
    play_toggle = false;
    send_response("CHARGING SOURCE DETECTED", 1);
  }

  if ((CHARGING_SOURCE_STATE == 0) && (OI_MODE_STATE == OI_PASSIVE) && (charge_toggle == true) && (play_toggle == false)){
    COIChangeMode(CMD_MODE_FULL);
    charge_toggle = false;
    send_response("FULL MODE ON", 1);
  }

  update_battery_sensors();
  charger();

  if (!digitalRead(INPUT_20) && !auto_cover ) { // check to see if not in script so command does not interfere
    //update_create_fast_sensors(); // so that collision detection works
    BUMPS_WHEEL_DROPS_STATE         = COIFetchSingleSensor(BUMPS_WHEELS);
    //charger();  // detects charger (once this works move make nest it deeper in framework)
  }

   if ((BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) && bp == false) {
          bp = true;
          send_state("collision detected");
        }
   if ((BUMPS_WHEEL_DROPS_STATE == 0) && bp == true) {
          bp = false;
          send_state("bumper released");
        }

  //heading = update_header(heading, COIFetchSingleSensor(ANGLE_SINCE_LAST); // Mostly for testing, for now
  delay(20); // serial gets overrun if no delay

}






