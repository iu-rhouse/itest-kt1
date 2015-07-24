#include <EEPROM.h>


////////////////////////////////////////
///
///   Utilities
///
////////////////////////////////////////


////////// katie id
int katie_id;
void get_katie_id() {
  char id[2];
  id[0] = EEPROM.read(0);
  id[1] = EEPROM.read(1);
  katie_id = atoi(id);
}



//////// cross header variables
int num_commands;// = 0;


////////////////////////////////////////
///
///   Send response to serial
///
////////////////////////////////////////
String form(int v) {
  return String(v) + " ";
}

String formB(byte v){
  // byte h = 1<<7;
  // int va = v ^ h; // subtracting 1<<7
  return String(v) + " ";
}

String formI(int v){
  // byte h = 1<<15;
  // int va = v ^ h; // subtracting 1<<15
  return String(v) + " ";
}

String formU(unsigned int v) {
  return String(v) + " ";
}

String formF(float f) {
   String p_s = "";
   p_s += (int)f;
   p_s += ".";
   int temp = (f - (int)f) * 100;
   p_s += abs(temp);
   return p_s + " ";
}

String response_startTag = "#msg";
void send_response(String r_string, int v){ 
  String response_string = "";
  response_string += response_startTag;
  response_string += " ";
  response_string += r_string;
  response_string += " ";
  response_string += form(v);
  Serial.println(response_string);
}

String state_startTag = "#stt";
void send_state(String r_string){ 
  String response_string = "";
  response_string += state_startTag;
  response_string += " ";
  response_string += r_string;
  Serial.println(response_string);
}

int convert_serial_to_int() {
  char cp[4];
  for (int j = 0; j < 4; j++) { // read an integer from -999 to 9999, must be 4 digits
    char c = Serial.read();
    if(isDigit(c) || c=='-') { // * does not catch eg '9-99'
      cp[j] = c;
    } 
  }
  int v = atoi(cp);
  return v;
}

int convert_n_serial_to_int(int n) {
  char cp[n];
  for (int j = 0; j < n; j++) { // read an integer from -999 to 9999, must be 4 digits
    char c = Serial.read();
    if(isDigit(c) || c=='-') { // * does not catch eg '9-99'
      cp[j] = c;
    } 
  }
  int v = atoi(cp);
  return v;
}

/************************************
 *          Reset Arduino
 ************************************/
//looks for the checksum, and if it is correct, resets the arduino
// void reset_arduino(){
//   if (Serial.available() > 0){
//     byte checksum = ~reset_command;
//     byte packet_checksum = Serial.read(); //the next byte
//     if ( checksum == packet_checksum){ //if the checksum is correct
//       //singAck();
//       num_commands = 0;
//       COITurnOffPlayLED ();   
//       COITurnOffAdvanceLED ();
//       COIDriveStop();
//       setup();        //and run the setup command again   
//     }
//   }
// }