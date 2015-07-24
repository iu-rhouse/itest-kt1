

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
const char dockc = 'd';
const char coverc = 'j';
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

///////////////////////// 
/// Command States
/////////////////////////
bool auto_on = false;
bool exp_auto = false;
bool auto_cover = false;


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
      case resetc:
        // resetArduino(); 
        // send_response("reset",4);
        break;

      case dockc:
        // need to stop commands (any new input shoud stop dock)
        //    need to turn off execute function (if docking)
        dock();
        break;

      case coverc:
        //if(!auto_cover) 
          COIDemo(COVER);

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

      case behaviore:
          //COIDemo(ABORT_CURRENT_DEMO);
         delay(50);  // make sure all commands to create have gone through...
         turnDegreeVelocityPIN(360,70);
         delay(150); // give time for pin to change 
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



 
    
