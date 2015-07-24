
void setup() {
  Serial.begin(115200);
}

void loop() {
  readSerialData();
}

// send a dummy message line to Serial
void sendSerialData(int ran){
  String packet_string = "";

  for (int i=0; i < 15; i++){
    packet_string += String(i*i*10+ran) + " ";
  } 

  Serial.println(packet_string);
}

// read the message line from Serial
void readSerialData(){
  int i=0;
  char commandbuffer[100];// we may need to increase this buffer size according to our maximum message string length
  if(Serial.available()){
    delay(100);// the magic!
    while(Serial.available() && i< 99) {
      commandbuffer[i++] = Serial.read();
      // test '\n' (EOL) will not work, as serial seems to give you extra '\n'!
    }
    commandbuffer[i++]='\0';
  }

  if(i>0){
    // pass the message line to the parser
    parseString(commandbuffer);
  }
}

// parse the received message line
void parseString(String message){
  // our message line format will be a sequential order of values separated by comma
  // each value is better to be an integer number as it does not help much to let Arduino work on strings/characters
  // for example: "200,501,1,25,-14,0,20,"
  if (message.length() > 0){
    int itemPosition = 0;
    int itemValueHolder[2];// we need to change this array length according to our maximum items allowed within a message
    char itemValueBuffer[10];
    int commaPosition = -1;

    do
    {
      commaPosition = message.indexOf(",");
      if(commaPosition != -1)
      {
        // separating items by comma
        String item = message.substring(0,commaPosition);

        item.toCharArray(itemValueBuffer, 10);
        int itemValue = atoi(itemValueBuffer);
        itemValueHolder[itemPosition] = itemValue;

        message = message.substring(commaPosition+1, message.length());
        itemPosition++;
      }
    }
    while(commaPosition >=0);

    // we have now parsed the message line into an integer array
    // for example, "200,501,1,25,-14,0,20," is now converted to [200,501,1,25,-14,0,20,]
    processMessage(itemValueHolder);
  }
}

// handle the parsed command/message
void processMessage(int itemValues[]){
  // according to different commands/message-types, we will have corresponding handler here
  
  // now this is just a dummy handling
  for (int i=0; i < itemValues[0]; i++){
    sendSerialData(i);
    delay(itemValues[1]);
  } 
}










