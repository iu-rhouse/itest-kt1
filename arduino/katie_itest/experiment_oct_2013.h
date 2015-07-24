


/********************************************************
 *
 *                   Sounds
 *
 *******************************************************/

/////// SCAN Sound Sequence

void scan_sound_sequence ( int k ) {
  note_t son[5];
  son[0].notePitch = 43 + k*2;
  son[0].noteDuration = 10;
  son[1].notePitch = 55 + k*2;
  son[1].noteDuration = 10;
  son[2].notePitch = 62 + k*2;
  son[2].noteDuration = 10;
  son[3].notePitch = 67 + k*2;
  son[3].noteDuration = 10;
//  son[4].notePitch = 71 + k*2;
//  son[4].noteDuration = 15;
  COIDefineNewSong (4, son, 4);
  delay(100);
  COIPlaySong(4);
}

void call_sound_sequence ( int k ) {
  note_t son[5];
  son[0].notePitch = 60 + k*2;
  son[0].noteDuration = 15;
  son[1].notePitch = 65 + k*2;
  son[1].noteDuration = 10;
 
  COIDefineNewSong (3, son, 2);
  delay(100);
  COIPlaySong(3);
}

/********************************************************
 *
 *                  Mock movements
 *
 *******************************************************/


void mock_scan(int k) {
  int turns = 8;
  int angle = 360 / turns;
  for (int i = 0; i < turns; i++){
    turnDegree(angle);
    delay(500);
    String st = "turn: ";
    st += String(angle);
    send_state(st);
    scan_sound_sequence(k);
    delay(1000);
  }
}

