
bool docking = false;
void dock() {
  
  docking = true;
  COIDemo(COVER_AND_DOCK);
}


void charger() {
  OI_MODE_STATE = COIFetchSingleSensor(OI_MODE);
  // Serial.print(OI_MODE_STATE);
  // Serial.print(" ");
  // Serial.print(CHARGING_SOURCE_STATE);
  // Serial.print(" ");
  // Serial.print(CHARGING_STATUS_STATE);
  // Serial.print(" ");
  // Serial.println(CURRENT_STATE);

  // When Katie moves onto dock / internal charger switch to safe mode... We need different loop for plug
  // if ((CHARGING_SOURCE_STATE > 0) && (OI_MODE_STATE == OI_FULL)) 
  //   // delay(100); // gives time for create to set chargin source state
  //   COIChangeMode(CMD_MODE_PASSIVE);

  // // If done charging on dock
  // if ((CHARGING_SOURCE_STATE == 2) && (OI_MODE_STATE == OI_PASSIVE) && (CHARGING_STATE == 3)) {
  //   COIChangeMode(CMD_MODE_SAFE);
  //   reverseScript();
  //   turnDegree(180);
  // }

  // // Create has docked so set robot docking state to off
  // if (CHARGING_SOURCE_STATE == 2) docking = false;


  // // If manually taken off of dock or unplugged from charger; exception for docking
  // if ((CHARGING_SOURCE_STATE == 0) && (OI_MODE_STATE == OI_PASSIVE) && (docking == false)){
  //   COIChangeMode(CMD_MODE_SAFE);
  // }

}


void update_create_fast_sensors(){
  // Note that each sensor takes 15ms to query from Create
  //  so 17 will take 1/4 of a second
  BUMPS_WHEEL_DROPS_STATE        	= COIFetchSingleSensor(BUMPS_WHEELS);
  WALL_STATE                     	= COIFetchSingleSensor(WALL);
  WALL_SIGNAL_STATE              	= COIFetchSingleSensor(WALL_SIG);
  CLIFF_LEFT_SIGNAL_STATE        	= COIFetchSingleSensor(CLIFF_LEFT_SIG);
  CLIFF_FRONT_LEFT_SIGNAL_STATE  	= COIFetchSingleSensor(CLIFF_FRONT_LEFT_SIG);
  CLIFF_FRONT_RIGHT_SIGNAL_STATE 	= COIFetchSingleSensor(CLIFF_FRONT_RIGHT_SIG);
  CLIFF_RIGHT_SIGNAL_STATE       	= COIFetchSingleSensor(CLIFF_RIGHT_SIG);
}

void update_create_slow_sensors() {
  VIRTUAL_WALL_STATE              = COIFetchSingleSensor(VIRTUAL_WALL);
  INFRARED_RX_STATE               = COIFetchSingleSensor(INFRARED_BYTE);
  BUTTONS_STATE                   = COIFetchSingleSensor(BUTTONS); // 
  VOLTAGE_STATE                   = COIFetchSingleSensor(BAT_VOLTAGE); //
  BATTERY_TEMP_STATE              = COIFetchSingleSensor(BAT_TEMP); //
  BATTERY_CHARGE_STATE            = COIFetchSingleSensor(BAT_CHARGE); //
}

void update_battery_sensors(){
  CHARGING_STATUS_STATE           = COIFetchSingleSensor(CHARGING_STATE);
  CURRENT_STATE                   = COIFetchSingleSensor(BAT_CURRENT);
  BATTERY_CAPACITY_STATE          = COIFetchSingleSensor(BAT_CAPACITY);
  CHARGING_SOURCE_STATE           = COIFetchSingleSensor(CHARGING_SOURCES);
  OI_MODE_STATE                   = COIFetchSingleSensor(OI_MODE);
}

// Send Create fast polling sensor data
String createfast_startTag = "#cfs";
void send_create_f_sensor_data() {
	String packet_string = ""; 
  packet_string += createfast_startTag;
  packet_string += " ";
  packet_string += formB(BUMPS_WHEEL_DROPS_STATE);
  packet_string += formI(WALL_SIGNAL_STATE);
 	packet_string += formI(CLIFF_LEFT_SIGNAL_STATE);
  packet_string += formI(CLIFF_FRONT_LEFT_SIGNAL_STATE);
  packet_string += formI(CLIFF_FRONT_RIGHT_SIGNAL_STATE);
  packet_string += formI(CLIFF_RIGHT_SIGNAL_STATE);
  Serial.println(packet_string);
}

// Send Create slow polling sensor data
String createslow_startTag = "#css";
void send_create_s_sensor_data() {
  String packet_string = ""; 
  packet_string += createslow_startTag;
  packet_string += " ";
  packet_string += formB(VIRTUAL_WALL_STATE);
  packet_string += formB(INFRARED_RX_STATE);
  packet_string += formB(BUTTONS_STATE);
  packet_string += formI(BATTERY_CHARGE_STATE);
  packet_string += form(BATTERY_TEMP_STATE);
  packet_string += formI(VOLTAGE_STATE);
  Serial.println(packet_string);
}

// Send Create encoder data, @e
String createsdist_startTag = "#dis";
void send_create_distance_data() {
  DISTANCE_STATE =  COIFetchSingleSensor(DIST_SINCE_LAST);
  ANGLE_STATE = COIFetchSingleSensor(ANGLE_SINCE_LAST);
  String packet_string = ""; 
  packet_string += createsdist_startTag;
  packet_string += " ";
  packet_string += form(DISTANCE_STATE);
  packet_string += form(ANGLE_STATE);
  Serial.println(packet_string);
}


// Send Create charger and battery sensor data, @c
String createscharge_startTag = "#chg";
void send_create_charging_data() {
  String packet_string = ""; 
  packet_string += createscharge_startTag;
  packet_string += " ";
  packet_string += form(CHARGING_STATUS_STATE); 
  packet_string += form(CURRENT_STATE);
  packet_string += form(BATTERY_CAPACITY_STATE);
  packet_string += form(CHARGING_SOURCE_STATE);
  packet_string += form(OI_MODE_STATE);
  Serial.println(packet_string);
}
