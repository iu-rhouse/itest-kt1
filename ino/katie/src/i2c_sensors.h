#include <i2cmaster.h>
// #include "communication.h" // for form() functions

#define MUXA_APIN  12
#define MUXB_APIN  8

float irtherm1 = 0, irtherm2 = 0;

void init_i2c_sensors() {

	i2c_init(); //Initialise the i2c bus
	PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
	Serial.println("I2C initialized");

  pinMode(MUXA_APIN,OUTPUT);
  pinMode(MUXB_APIN,OUTPUT);
  
  /// set i2c mux to talk to compass
  digitalWrite(MUXB_APIN, HIGH); 
  digitalWrite(MUXA_APIN, HIGH);
}


String i2c_startTag = "#itc";
void send_i2c_sensor_data() {
  String packet_string = "";
  packet_string += i2c_startTag;
  packet_string += " ";
  packet_string += formF(irtherm1);
  packet_string += formF(irtherm2);
  packet_string += formF(irtherm1); // empty
  packet_string += formF(irtherm2); // sonar
  Serial.println(packet_string);
}


/********************************************************
 *                   IR Therm
 *******************************************************/
//utility function for calculating the ir thermometer temperature
float vIRTherm(){
  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;

  float celcius = tempData - 273.15;
  float fahrenheit = (celcius*1.8) + 32;
//  Serial.println(fahrenheit);
  return fahrenheit;

}

void update_i2c_sensors() {
  irtherm1 = 0, irtherm2 = 0;

  //set the pins for the multiplexer
  digitalWrite(MUXA_APIN,HIGH);
  digitalWrite(MUXB_APIN,LOW);
  //read from the multiplexer
  // Serial.println("IR therm 1...");
  irtherm1 += vIRTherm();
  
  //set the pins for the multiplexer
  digitalWrite(MUXA_APIN,LOW);
  digitalWrite(MUXB_APIN,LOW);
  // Read from the multiplexer
  // Serial.println("IR therm 2...");
  irtherm2 += vIRTherm();

}

