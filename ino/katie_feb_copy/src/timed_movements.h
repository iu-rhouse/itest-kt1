//timed_movements.h


unsigned long move_timer = 0;
unsigned long move_time = -1;

// angle and velocity
void start_pivot(int a, int v){

  // calculate the circumference (in mm) and then divide by velocity
  // will give us the time it takes to make the turn. 
  //  How far apart are the wheels on the Create? 240.7mm ...120mm
  //   and so 360 degrees = 754mm
  float t = ((a/360)*754)/v;
  move_time = t * 1000;

  // set clockwise or counter clockwise , verify these
  if (a < 0) {
  	COIDirectDrive(v, -v);
  } else {
  	COIDirectDrive(-v, v);
  }
  move_timer = millis(); // may need to be micros..., verify

}

// takes distance and velocity
void start_distance(int d, int v){
  float t = d/v;
  move_time = t * 1000;

  // set forward or reverse
  if (d < 0) {
  	COIDirectDrive(-v, -v);
  } else {
  	COIDirectDrive(v, v);
  }
  move_timer = millis(); // may need to be micros..., verify
}



void execute_move() {
	if (move_time != 0) { // no need to waste time if no move is happening
		if (millis() - move_timer > move_time) {
			COIDirectDrive(0,0);
			move_time = 0;
		}
	}
}






