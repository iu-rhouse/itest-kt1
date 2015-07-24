bool debug = false;

//////////// BEHAVIOR PARAMETERS //////////
const int space_size = 360; //cm
const int over_shoot_angle = 3; // on each angle turn
const int max_scan_divisions = 18;
int scan_divisions = 8; // 30degrees each


////////// TEMPORAL CYCLES ///////////
int hypermeter = 120; 
int midmeter = 80; 
int submeter = 12;


int scan_timer = submeter; 
int move_size = midmeter; 
int rest_timer = hypermeter;
int move_timer = hypermeter; 

bool collision = true;
int collisions = 1;

int rear_range_D[max_scan_divisions];  // best fit is 0 + 8 or 9
  //  direciton fomr katie heading of rear distance sensor


int calculateDelay(int d, int v) {
  int s = ( d / v ) * 1000;
  return s; // return milliseconds to execute...
}


void wall_follow(int follow_on) {
  int bump  = COIFetchSingleSensor(BUMPS_WHEELS);
  int wall = COIFetchSingleSensor(WALL);

  if (follow_on > 0) {
    if (bump > 0 && bump < 4) {
      COIDrive(300,0x0001); // r = .3
    } else {
      if (wall == 1) {
        COIDrive(300,150); // t = .3, r = .15
      } else {
        COIDrive(250,-670); // t = .25, r = -.37 
      }
    }
  } else {
    COIDrive(0,0);
  }
}



// Type 0 is a basic, type 1 is list with 2 tabs
void debug_print( int dat, String id, int type ) {
  if (debug == true) {
    if (type == 0) {
      Serial.println();
      Serial.print(id);
      Serial.print(" ");
      Serial.print(dat);
      Serial.println();
    }
    else if (type == 1) {
      Serial.print(dat);
      Serial.print("    ");
    }
  }
}

int generate_cycle(int meter, float ratio) {
  int r;
  r = ((ratio * meter) + random(((1 - ratio) + 1) * meter));
  debug_print(r, "new cycle:", 0);
  return r;
}

int world_scale(int world_size, float travel_ratio, float variation_ratio) {
  int r;
  int w = world_size * travel_ratio;
  r = ((variation_ratio * w) + random(((1 - variation_ratio) + 1) * w));
  debug_print(r, "new distance:", 0);
  return r;
}

void directions_scan(int robot_D) {
  int turns = scan_divisions;
  int angle = 360 / turns;
  for (int i = 0; i < turns; i++){
        
      //if (robot_D > 11) { robot_D = 0; }
         /// do 10 readings...
        int temp = 0;
        int num_of_readings = 30;
        for (int j = 0; j < num_of_readings; j++) {
          delay(20);
          int sensor_value = analogRead(REARRANGEPIN); 
          if (sensor_value < 0 || sensor_value > 1000) { j--; }
          else { temp += sensor_value; }
        }
        delay(20);
        // int sensor_D = robot_D + 6; // rear range is at postion 180 from front of robot
        // if (sensor_D > 11) { sensor_D = sensor_D - 11; }
        rear_range_D[robot_D] = temp / num_of_readings;
          debug_print(rear_range_D[robot_D],"",1);
          // Serial.print(rear_range_D[robot_D]);
          // Serial.print("    ");
        turnDegreeVelocity(angle - over_shoot_angle, 100);
        //katie_D = katie_D + (angle - over_shoot_angle); 

        int d = 1000 / turns;
        delay(calculateDelay(d,100));
        robot_D++;

        // data_to_RPI();
  }
  // scan_sound_sequence(2);
  if (debug) {Serial.println(" -- ");}
}

int move_stop() {
  COIDriveStop();
  num_commands = 0;
  goal_distance = 0; 
  distance_traveled = 0;
  angle_turned = 0;
  goal_angle = 0;
}


bool bump_stop() {
  int bump = COIFetchSingleSensor(BUMPS_WHEELS);
  if ( bump > 0 && bump < 4) { 
    move_stop(); 
    return true;
    } else {
      return false;
    }
}

void move_forward( int d) {
  bool completed = false;

  while (completed == false) {
    if( bump_stop() ) { completed = true; }

  }
}

int best_open_direction() {
  int rear_range_D2[scan_divisions];
  int best_direction;
  for ( int i = 0 ; i < scan_divisions ; i++ ) {
    int temp = rear_range_D[i];
    if ( i == 0 ) { 
      temp += rear_range_D[scan_divisions - 1];
    } else {
      temp += rear_range_D[i - 1];
    }

    if ( i == 11 ) {
      temp += rear_range_D[0];
    } else {
      temp += rear_range_D[i + 1];
    }

    rear_range_D2[i] = temp / 3;

  }

  best_direction = 1;
  for (int i = 0; i < scan_divisions; i++ ) {
    if (rear_range_D2[i] < rear_range_D2[best_direction]) {
      if (collision == 1 && i == 0) { } else {
      best_direction = i; }
    }

  }

  debug_print(rear_range_D2[best_direction], "lowest value:", 0);
  debug_print(best_direction, "best direction:",0);


  return best_direction;
}



int go_in_direction_of_lowest(int robot_D) {

  int candidate_D = 1;

  candidate_D = best_open_direction();

  int inv_candidate_D = candidate_D + (scan_divisions / 2);
  if (inv_candidate_D > scan_divisions - 1) {
    inv_candidate_D = inv_candidate_D - scan_divisions; 
  }
          
  int angle = (inv_candidate_D * (360 / scan_divisions)) - over_shoot_angle;


  if (angle > 180) {
    angle = 360 - angle;
  }

  debug_print(angle, "Turn angle is:", 0);

        
  turnDegree(angle);
  int d = angle / 360 * 1000;
  delay(calculateDelay(d, default_turn_velocity));
  // forward_command = 1;

  return candidate_D;
}

void forward_with_collision_stops(int distance_mm){
 
  if (BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) {
//  if (bump_stop) {
    num_commands = 0;
    reverseScript();
    collision = true;
    collisions++;
    if (debug) { Serial.println("Collision"); }
    delay(500);
  }
  if (BUMPS_WHEEL_DROPS_STATE == 4) {
    num_commands = 0;
    move_stop();
    if (debug) { Serial.println(" Wheel above 3.  "); }
  }
    
  if ( num_commands > 0 ) {
    bool is_distance_finished = execute_distance(distance_mm);

    if(is_distance_finished) { 
      num_commands = 0;
      collision = false;
      collisions = 0;
      if (debug) { Serial.println("Distance finished."); }
    }
  } else {
    move_stop();
  }
}

void katie_auto() {

  //updateCreateSensor();

  if (rest_timer < 0 ) {

    if (scan_timer == 0) {
        Serial.println();
        if (collision) {
          scan_divisions = 8;
        } else {
          scan_divisions = 3;
        }
        
        directions_scan(0);
        go_in_direction_of_lowest(0); 
      
        scan_timer = generate_cycle(submeter, .3); 

        float travel_length = .2;// one twentieth the room size is median
        if (random(10) > 5 && collision == false) { travel_length = .6; } // 1 out of 2 will result in further range...
        move_size = world_scale(space_size, travel_length, .3);
        // if (collision) { move_size = midmeter; }
        //collision = 0;
        num_commands = 1;
        delay(50); // delay so that jolt from stopping doesn't trigger bumper
    }

    
    // in scan loop
    if (num_commands == 0){ 
      scan_timer--;
      move_timer--;
      if (debug) {
        Serial.print(scan_timer);
        Serial.print(".");
      } 
      update_create_fast_sensors();
    } else {
      if (collisions > 2) {
        wall_follow(move_size);
        move_size--;
        if (move_size == 0) { 
          num_commands = 0; 
          collisions = 1;
        }
      } else {
        forward_with_collision_stops(move_size);
      }
    } 

    // if (num_commands == 0) { move_timer--; }

    if (move_timer == 0) {
      if (debug) { Serial.print("rest_timer - "); }
      rest_timer = generate_cycle(hypermeter, .6);
    }
  }
  else if (rest_timer > 0) {
    rest_timer--;
//    debug_print( rest_timer, "rest timer:", 0 );
  }
  else { 
    if (debug) {Serial.print("move_timer - ");}
    move_timer = generate_cycle(midmeter, .6); // old was hypermeter / 3
    rest_timer = -1; 
  }

}