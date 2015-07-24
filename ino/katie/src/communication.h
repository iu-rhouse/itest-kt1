////////////////////////////////////////
///
///   Execute a sequence of moves
///
////////////////////////////////////////
char command_parameter[6];  
char cmdp[1];
// int num_commands;// = 0; /// in utilities
int command_buffer [30];
int command_cursor = 0;
unsigned long time_out = 0;

void read_command_sequence(){
  cmdp[0] = Serial.read();
  num_commands = atoi(cmdp);
  // Serial.print("number of commands: ");
  // Serial.println(num_commands);
  for (int i =0; i < num_commands; i++){
    command_buffer[i] = convert_serial_to_int();
    // send_response("command_buffer", command_buffer[
  //num_commands = 0;
  command_cursor = 0; // set global iterator to first move
  time_out = 0; /// I do not remember if implemented  ...take a look
  }
// echo
  }

////////////////////////////////////////
///
///   Stream data
///
////////////////////////////////////////
#define INPUT_20            5
bool stream = false;
void stream_data() {

    if (!digitalRead(INPUT_20))
      update_create_fast_sensors();
    send_create_f_sensor_data();
    
    update_i2c_sensors();
    send_i2c_sensor_data();

    update_analog_sensors();
    send_analog_sensor_data();
}


void stream_data_new() {
  stream_data();

}

bool exp_stream = false;
// stream function defined in separate header


bool debug_stream = false;
void stream_debug_sensors() {
    update_battery_sensors();
    send_create_charging_data();
}

////////////////////////////////////////
///
///  Recieve command sentences from serial
///
////////////////////////////////////////
char HEADER = 'null';
char COMMAND = 'null';
const char HEAD_command_stream = '$';
const char HEAD_single_command = '@';
// const char HEAD_move_command = '%';
const char dockc = 'k';
const char distancec = 'd'; // send distance data
const char coverc = 'j';
const char abortc = 'q';
const char resetc = 'r';
const char autoc = 'u';
const char autoe = 'a';
const char scanc = 's';
const char beepc = 'b';
const char create_fast_poll = 'f';
const char create_slow_poll = 'l';
const char analog_poll = 'n';
const char i2c_poll = 'i';
const char streamd = 't';  // primary stream
const char streamb = 'g';  // debug stream
const char streame = 'e';  // experiment streams
const char behaviore = 'h'; // experiment behavior
const char chargec = 'c';
const char mode_toggle = 'z';

/// These two are followed by two values
const char straight = 'x'; // drive straight
const char pivot = 'p'; 
const char around = 'v';
const char halt = 'w';

Servo camera_arm;
const char camera = 'm';

///////////////////////// 
/// Command States
/////////////////////////
bool auto_on = false;
bool exp_auto = false;
bool auto_cover = false;

int d,a,v,r;


////////////////
/// CHARGING STATES
////////////////
boolean charge_toggle = false;
boolean play_toggle = false;


void(* resetFunc) (void) = 0;//declare reset function at address 0


void recieve_commands(){
  //mycommand = false;
  //reads through the serial data to see if we have a header match
  while (Serial.available() > 0){
    HEADER = Serial.read();
    // This is basically only used for sequences of angle and distance pairs
    if (HEADER == HEAD_command_stream){
      read_command_sequence();
    }
    // This will be a long switch statement for application specific commands and
    //    paramter changes to whatever model we are running.
    if (HEADER == HEAD_single_command){
      COMMAND = Serial.read();
      switch(COMMAND){
      case camera:camera_arm.attach(9,90,180);
        
        delay(200);
        r = convert_serial_to_int();
        camera_arm.write(r);
        delay(400);
        camera_arm.detach();
      case straight:
        d = convert_serial_to_int() * 10;
        v = convert_serial_to_int();
        start_straight(d,v);
        break;
      case pivot:
        a = convert_serial_to_int();
        v = convert_serial_to_int();
        start_pivot(a,v);
        break;
      case around:
        d = convert_serial_to_int() * 10;
        r = convert_serial_to_int() * 10;
        v = convert_serial_to_int();
        start_rounded(d,r,v);
        break;
      case halt:
        COIDirectDrive(0,0);
        break;
      case resetc:
        //setup();
        // resetArduino(); 
        // send_response("reset",4);
        resetFunc();
        break;

      case mode_toggle:
        update_battery_sensors();
        if (OI_MODE_STATE == OI_PASSIVE || OI_MODE_STATE == OI_SAFE) {
          COIChangeMode(CMD_MODE_FULL);
          play_toggle = false;
        }
        else {
          COIChangeMode(CMD_MODE_SAFE);
          play_toggle = true;
        }
        break;

      case dockc:
        if (OI_MODE_STATE == OI_PASSIVE || OI_MODE_STATE == OI_SAFE) {
          COIChangeMode(CMD_MODE_FULL);
          //start_pivot(180,200);
        } else {
          COIChangeMode(CMD_MODE_SAFE);
          dock();
        }
        break;


      case coverc:
        COIDemo(COVER);
        break;

      case abortc:
        COIDemo(ABORT_CURRENT_DEMO);
        break;

      case distancec:
        send_create_distance_data();
        break;

        if(auto_cover)
          COIDemo(ABORT_CURRENT_DEMO);
        auto_cover = !auto_cover;
        break;

      case autoc:
        auto_on = !auto_on;
        // if state changes , need to reset some variables...
        // send_response("auto",5);

        break;

      case autoe:
        exp_auto = !exp_auto;
        break;

      case scanc:
        mock_scan(katie_id);
        break;

      case beepc:
        call_sound_sequence(katie_id);
        break;

      case create_fast_poll:
        update_create_fast_sensors();
        send_create_f_sensor_data();
        break;

      case create_slow_poll:
        update_create_slow_sensors();
        send_create_s_sensor_data();
        break;

      case analog_poll:
        update_analog_sensors();
        send_analog_sensor_data();
        break;

      case i2c_poll:
        update_i2c_sensors();
        send_i2c_sensor_data();
        break;

      case streamd:
        // toggle stream switch
        stream = !stream;
        break;

      case streamb:
        debug_stream = !debug_stream;
        break;

      case streame:
        exp_stream = !exp_stream;
        break;

      case behaviore: //Adjusted to (hopefully) take pictures every 30 degrees
          //COIDemo(ABORT_CURRENT_DEMO);
         delay(50);  // make sure all commands to create have gone through...
         for(int i = 1; i <= 12; i++){
           stream_data(); //What I assume captures the data?
           turnDegreeVelocity(30,70); // turn 30 degrees after taking pic
           delay(2000); //For the sake of operation
         }
        // turnDegreeVelocityPIN(360,70);
        // delay(150); // give time for pin to change 
        break;

      case chargec:
        update_battery_sensors();
        send_create_charging_data();
        break;

      default: break;

      }
    }
  }
}



 
    
