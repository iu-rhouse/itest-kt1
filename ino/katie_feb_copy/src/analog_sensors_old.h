/********************************************************
 *
 *                   Arduino Sensor Loop
 *
 *******************************************************/
#define LIGHTPIN A0
#define THERMPIN A1 // changes to UPWARDRANGEPIN
#define REARRANGEPIN A2
#define PIEZOPIN A3
 //// Names are vestiges since these are changing quickly.


//reads the arduino sensors for a specified loop count
//sends the data to the serial port
int piezoret = 0, thermret = 0, lightret = 0, rear_range_ret = 0;
// int multiplexret = 0, multiplexret2 = 0;

void update_analog_sensors()
{ 
  	piezoret = 0, thermret = 0, lightret = 0, rear_range_ret = 0;
  	// multiplexret = 0, multiplexret2 = 0;
  	piezoret       += analogRead(PIEZOPIN); // 3
  	thermret       += analogRead(THERMPIN);  // 1
  	lightret       += analogRead(LIGHTPIN);    // 0
  	rear_range_ret += analogRead(REARRANGEPIN);  // 2
}

void send_analog_sensor_data() {
    String packet_string = "";
//  packet_string += form(startTag)
    packet_string += form(piezoret);
    packet_string += form(thermret);
    packet_string += form(lightret);
    packet_string += form(rear_range_ret);
    Serial.println(packet_string);
}

void send_analog_sensor_data_legacy_ordering() {
  	String packet_string = "";
//  packet_string += form(startTag)
  	packet_string += form(piezoret);
  	packet_string += form(thermret);
  	packet_string += form(lightret);
  	packet_string += form(rear_range_ret);
  	Serial.println(packet_string);
}

