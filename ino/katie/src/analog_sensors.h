/********************************************************
 *
 *                   Arduino Sensor Loop
 *
 *******************************************************/
#define PIN0 A0
#define PIN1 A1 
#define PIN2 A2
#define PIN3 A3

 #define REARRANGEPIN PIN2

//// Following names are vestiges since these are changing quickly.
int piezoret = 0, thermret = 0, lightret = 0, ultra_range = 0;

void update_analog_sensors()
{ 
  	piezoret = 0, thermret = 0, lightret = 0, ultra_range = 0;
  	piezoret       += analogRead(PIN0);
  	thermret       += analogRead(PIN1);  
  	lightret       += analogRead(PIN2);   
  	ultra_range    += analogRead(PIN3);  
}

String analog_startTag = "#ana";
void send_analog_sensor_data() {
    String packet_string = "";
    packet_string += analog_startTag;
    packet_string += " ";
    packet_string += form(piezoret); // PIN 0
    packet_string += form(thermret);  // PIN 1
    packet_string += form(lightret);   // PIN 2
    packet_string += form(ultra_range);   // PIN 3
    Serial.println(packet_string);
}


