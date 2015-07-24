//script 1 a
void run_script_1() {
  
}

//script 2 e
void run_script_2() {
  
}

//script 3 g
void run_script_3() {
  
}

//script 4 h 
void run_script_4() {
  
}

//beep alternative i
void beep_2(){
  note_t son[5];
  son[0].notePitch = 30 + 12;
  son[0].noteDuration = 75;
  son[1].notePitch = 55 + 12;
  son[1].noteDuration = 25;
 
  COIDefineNewSong (3, son, 2);
  delay(100);
  COIPlaySong(3);
}

//generic function 1 j // attached to play button >
void function_1() {
  forward(8);
  left(90);
  forward(4);
}

//generic function 2 k // and attached to advance button >>
void function_2() {
//  left(1);
//  right(2);
//  forward(2);
  back(2);
}

//generic function  s
void function_3() {
  
}

//generic function 4 l
void function_4() {
}

//generic function 5 u
void function_5() {
  
 
}





