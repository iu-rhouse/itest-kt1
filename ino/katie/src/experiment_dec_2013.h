//  experiment_dec_2013.h


////  depends on i2c sensors.h , create sensors.h and analog sensors.h
void arduino_hub_stream() {
  String packet_string = "";

  update_i2c_sensors();
  packet_string += formF(irtherm1);
  packet_string += formF(irtherm2);

  update_analog_sensors();
  packet_string += form(piezoret); // PIN 0
  packet_string += form(thermret);  // PIN 1
  packet_string += form(lightret);   // PIN 2
  packet_string += form(ultra_range);   // PIN 3
 
  if (!digitalRead(INPUT_20)) update_create_fast_sensors();
  packet_string += formB(BUMPS_WHEEL_DROPS_STATE);
  packet_string += formI(WALL_SIGNAL_STATE);
  packet_string += formI(CLIFF_LEFT_SIGNAL_STATE);
  packet_string += formI(CLIFF_FRONT_LEFT_SIGNAL_STATE);
  packet_string += formI(CLIFF_FRONT_RIGHT_SIGNAL_STATE);
  packet_string += formI(CLIFF_RIGHT_SIGNAL_STATE);

  Serial.println(packet_string);

}




// bool start_flag = false;
// unsigned long start_time = 0;
// unsigned long check_time = 0;


// void experiment_scanning2() {
  
//   int angle = 30;
//   if (check_time < 20000) {
//     check_time = millis() - start_time;
//   } else {
//     if (execute_turn(angle) == true) {
//       start_time = millis();
//       check_time = 0;
//     }
//   }
  
// }




