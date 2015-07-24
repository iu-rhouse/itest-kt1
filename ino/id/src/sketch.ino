  #include <EEPROM.h>
  
  const int idLength = 4;
  
  void setup()
  {
    Serial.begin(57600);
    readId(idLength);
    Serial.println("Please input the two bytes Katie id and two bytes favorite number for this Katie without space:");
  }
  
  void loop()
  {
    delay(50);
    readInput(idLength-1);
  }
  
  void readInput(int length){
    int incomingByte;
    incomingByte = Serial.read();
    if(incomingByte==-1){
      return;
    }
  
    int startIndex = 1;
    char valueBuffer[length-1];
    valueBuffer[0] = incomingByte;
  
    while (incomingByte != -1){
      // read the incoming byte:
      incomingByte = Serial.read();
      if (startIndex > length){
        break;
      }
  
      valueBuffer[startIndex] = incomingByte;
      startIndex++;
    }
  
    if(valueBuffer[0] != ','){
      writeId(idLength, valueBuffer);
    }
    readId(idLength);
  }
  
  void writeId(int length, char valueBuffer[]){
    for (int i = 0; i < length; i++){
      EEPROM.write(i, valueBuffer[i]);
    }
    Serial.println("Write EEPROM finish");
  }
  
  void readId(int length){
    Serial.println("Current id on EEPROM is: ");
    for (int i = 0; i < length; i++){
      int value = EEPROM.read(i);
      Serial.print((char) value);
    }
    Serial.println();
  
    char id[2];
    id[0] = EEPROM.read(0);
    id[1] = EEPROM.read(1);
    Serial.println(atoi(id));
  
    id[0] = EEPROM.read(2);
    id[1] = EEPROM.read(3);
    Serial.println(atoi(id));
  }