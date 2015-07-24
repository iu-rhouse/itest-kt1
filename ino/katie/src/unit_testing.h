

void unit_tests() {

	BUMPS_WHEEL_DROPS_STATE        	= COIFetchSingleSensor(BUMPS_WHEELS);
	Serial.print(BUMPS_WHEEL_DROPS_STATE);
	Serial.print("\t\t");
	Serial.println(BUMPS_WHEEL_DROPS_STATE,BIN);

	COIRecieveAndUpdateSensor(BUMPS_WHEELS);
	Serial.print(BUMPS_WHEEL_DROPS_STATE);
	Serial.print("\t\t");
	Serial.println(BUMPS_WHEEL_DROPS_STATE,BIN);


	COISerialWriteByte (CMD_SENSORS);
    COISerialWriteByte (BUMPS_WHEELS);
    byte high = COISerialReadByte ();
    Serial.print(high);
	Serial.print("\t\t");
	Serial.println(high,BIN);

	COISerialWriteByte (CMD_SENSORS);
    COISerialWriteByte (BUMPS_WHEELS);
    if (!COIWaitForData ()) {
    	Serial.println("error");
    } else {
    	byte high = COISerialReadByte ();
    	Serial.print(high);
		Serial.print("\t\t");
		Serial.println(high,BIN);
	}
}

void unit_tests_2() {

        BUMPS_WHEEL_DROPS_STATE         = COIFetchSingleSensor(BUMPS_WHEELS);
        Serial.print(BUMPS_WHEEL_DROPS_STATE);
        Serial.print("\t\t");
        Serial.println(BUMPS_WHEEL_DROPS_STATE,BIN);

        byte a = 1<<7;
        Serial.println(a,BIN);
        byte out = BUMPS_WHEEL_DROPS_STATE ^ a;
        Serial.println(out,BIN);


        CLIFF_LEFT_SIGNAL_STATE        	= COIFetchSingleSensor(CLIFF_LEFT_SIG);
        Serial.print(CLIFF_LEFT_SIGNAL_STATE);
        Serial.print("\t\t");
        Serial.println(CLIFF_LEFT_SIGNAL_STATE);

        byte b = 1<<15;
        out = CLIFF_LEFT_SIGNAL_STATE ^ b;
        Serial.println(CLIFF_LEFT_SIGNAL_STATE);

        // NOTES
        //   The flipped high bit is still odd, not consistent with documentation
        //   Inverting the bit works for limited cases but, if I don't know,
        //	 why this is happening it is impossible to write a robust method 
        //	 for converting Create units to Arduino 

        // Checked with setup without shield (only aarduino and create) and units worked as expected
}