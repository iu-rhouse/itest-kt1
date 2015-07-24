//timed_movements.h


unsigned long move_timer2 = 0;
unsigned long move_time = 0;

// give robot 500 milliseconds to move out of
// collision position where bumper is depressed
unsigned long bump_timer = 500; 
unsigned int vel = 200;
unsigned int tile = 250;

// angle and velocity
void start_pivot(float a, float v){

  // calculate the circumference (in mm) and then divide by velocity
  // will give us the time it takes to make the turn. 
  //  How far apart are the wheels on the Create? 262mm ...131mm
  //   and so 360 degrees = 823mm
  float b = abs(a);
  float t = ((b/360)*823)/v;
  move_time = t * 1000;
  // Serial.print(t);
  // Serial.print(" ");
  // Serial.println(move_time);

  // set clockwise or counter clockwise , verify these
  if (a < 0) {
  	COIDirectDrive(v, -v);
  } else {
  	COIDirectDrive(-v, v);
  }
  move_timer2 = millis(); // may need to be micros..., verify

}

void start_rounded(float d, float r, float v){
  float b = abs(d);
  float t = b/v;
  move_time = t * 1000;

  // set forward or reverse
  if (d < 0) {
  	COIDrive(-v, r);
  } else {
  	COIDrive(v, r);
  }
  move_timer2 = millis(); // may need to be micros..., verify
}

// takes distance and velocity
void start_straight(float d, float v){
  float b = abs(d);
  float t = b/v;
  move_time = t * 1000;

  // set forward or reverse
  if (d < 0) {
  	COIDirectDrive(-v, -v);
  } else {
  	COIDirectDrive(v, v);
  }
  move_timer2 = millis(); // may need to be micros..., verify
}



void execute_move() {
       
  
  
	if (move_time != 0) { // no need to waste time if no move is happening
		unsigned long mt = millis() - move_timer2;


		if (mt > move_time || ((BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) && bump_timer < mt)) {
			COIDirectDrive(0,0);
                        send_state("timed_move_completed");
                        send_create_distance_data();
                        send_create_f_sensor_data();

			
			// Serial.print("done: ");
			// Serial.print(mt);
			// Serial.print(" over by ");
			// int diff = mt - move_time;
			// Serial.println(diff);

			move_time = 0;

		}
  }
}

int update_heading(int heading, int angle){
	heading = heading + (angle % 360);
	if(heading < 0){
		heading = 360 + heading;
	} else if(heading > 360){
		heading = heading - 360;
	}
	return heading;
}		

		// int d;
  //   	d = COIFetchSingleSensor(DIST_SINCE_LAST);
  //   	int a;
  //   	a = COIFetchSingleSensor(ANGLE_SINCE_LAST);
  //   	Serial.print(d);
  //   	Serial.print(" - ");
  //   	Serial.println(a);

// 	}
// }

// void slow_down(){
// 	if (move_time != 0) {
// 		unsigned long mt = millis() - move_timer2;
// 		//when last x seconds of function are remaining
// 	}
// }



/////
/////
/////   MOVE FUNCTIONS FOR BASIC SCRIPTS
/////
/////
/////

void set_velocity(float v){
 vel = v; 
}

void straights(float d, float v) {
  start_straight(d,v);
  delay(move_time);
}

void forward(float d){
  d = abs(d) * tile;
  straights(d, vel);
}
void back(float d){
  d = abs(d) * tile;
  straights(d * -1, vel);
}

void pivots(float a, float v ) {
  start_pivot(a,v);
  delay(move_time);
}

void right(float a){
  pivots(abs(a), vel);
}

void left(float a){
  pivots(abs(a) * -1, vel);
}


void arcs(float d, float r, float v) {
  start_rounded(d,r,v);
  delay(move_time);
}

