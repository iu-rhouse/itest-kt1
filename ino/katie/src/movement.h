/********************************************************
 *
 *                   Execute move commands
 *
 *******************************************************/
// Receive command from Serial and save it to command buffer
// int command_buffer [30];
int temp_command_buffer [30];
int max_360_time = 1500; //1.2 seconds ...?

//variable for executing commands
unsigned long  prev_query_time   = 0;
int   goal_angle        = 0;
float angle_turned      = 0;
int   goal_distance     = 0;
int  distance_traveled = 0;
int velocity = 100;



/********************************************************
 *                   Execute Distance
 *******************************************************/
// Let Katie go straight until it has moved certain distance
// Input:  Distance
// Output: if Katie has travel that distance

// Go forward, backward or stop moving depending on the goal_distance
void start_move_straight(){
  if(goal_distance > 0)        COIDirectDrive(velocity, velocity);
  else if (goal_distance < 0)  COIDirectDrive(-velocity, -velocity);
  else                                           return;
}

// Reset the goal and traveled distance
void reset_distance(){
  goal_distance = 0;
  distance_traveled = 0;
}

// Return true if has travled goal distance
bool finished_distance(){
  if(abs(distance_traveled) >= abs(goal_distance)) 
    return  true;
  else return false;
}

// Update the distance traveled in certain time
void update_distance(int update_time){
  if ((millis() - prev_query_time) > update_time){
    prev_query_time = millis();
    // byte x = 1<<15;
    int x;
    x = COIFetchSingleSensor(DIST_SINCE_LAST); // ^ x; //millimeters 
    distance_traveled += x;
    // send_response("Distance so far: ", distance_traveled);
  }
}

bool execute_distance(int _goal_distance)
{
  if(distance_traveled==0){
    goal_distance = _goal_distance;
    distance_traveled = 0;
    // send_response("Starting travel distance: ", goal_distance);
    int d = COIFetchSingleSensor(DIST_SINCE_LAST);
    // send_response("Distance since last: ", d);
    start_move_straight();
  }
  update_distance(150);// update distance every 300 milli seconds

  if(finished_distance()){
    COIDriveStop(); //stop
    reset_distance();
    return true;
  }    
  return false;
}


void interaction() {
  if(BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) {
    
  }

}

void interaction_old() {
  // if robot is not moving ...
  if (num_commands == 0){
    if(BUMPS_WHEEL_DROPS_STATE == 1) {
      // right bumper
      num_commands = 6;
      command_buffer[0] = random(90,120);
      command_buffer[1] = 200;
      command_buffer[2] = random(100,400);
      command_buffer[3] = 180; /// turn back to face kicker
      command_buffer[4] = 200;
      command_buffer[5] = 0;
      command_cursor = 0;
    }
    else if (BUMPS_WHEEL_DROPS_STATE == 2) {
      // left bumper
      num_commands = 6;
      command_buffer[0] = random(-90,-120);
      command_buffer[1] = 200;
      command_buffer[2] = random(100,400);
      command_buffer[3] = 180; /// turn back to face kicker
      command_buffer[4] = 200;
      command_buffer[5] = 0;
      command_cursor = 0;
    }
    else if (BUMPS_WHEEL_DROPS_STATE == 3) {
      // right bumper
      num_commands = 6;
      command_buffer[0] = random(170,190);
      command_buffer[1] = 200;
      command_buffer[2] = random(100,400);
      command_buffer[3] = 180; /// turn back to face kicker
      command_buffer[4] = 200;
      command_buffer[5] = 0;
      command_cursor = 0;
    }

  }
  else {
    if (BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) {
      call_sound_sequence(2);
    num_commands = 0;
    command_cursor = 0;
    //reverseScript();
    //delay(250);
    send_state("bump and backup");
    }
    if (BUMPS_WHEEL_DROPS_STATE > 3) {
      call_sound_sequence(3);
      num_commands = 0;
      command_cursor = 0;
    }
  }



}



void execute_commands(){
  
  // if (BUMPS_WHEEL_DROPS_STATE > 0 && BUMPS_WHEEL_DROPS_STATE < 4) {
  //   num_commands = 0;
  //   reverseScript();
  //   delay(250);
  //   send_state("bump and backup");
  // }
  // if (BUMPS_WHEEL_DROPS_STATE > 3) {
  //   num_commands = 0;
  // }
    
  //if we have commands that haven't been executed
  if (0<=command_cursor && command_cursor < num_commands ){

    //0 remainder sets angle 
    if (command_cursor%3 == 0){
      if ( time_out == 0 ) { 
        turnDegree(command_buffer[command_cursor]);
        time_out = millis();
        // send_response("Turn", command_buffer[command_cursor]);

      }
      if((millis() - time_out) > max_360_time) {
        String st = "turn: ";
        st += String(command_buffer[command_cursor]);
        send_state(st);
        command_cursor++;  
        time_out = 0;
      }
    }
    // 1 remainder sets velocity 
    else if ((command_cursor-1)%3 == 0){
      velocity = command_buffer[command_cursor];
      // send_response("Set velocity", command_buffer[command_cursor]);
      command_cursor++;
    }
    // 2 remainder sets distance
    else if ((command_cursor-2)%3 == 0){
      bool is_distance_finished = execute_distance(command_buffer[command_cursor]);
      if(is_distance_finished) { 
        // send_response("Distance completed", command_buffer[command_cursor]);
        String st = "distance: ";
        st += String(command_buffer[command_cursor]);
        send_state(st);
        command_cursor++;
        time_out = 0; // if there is another leg to turn.
      }
    }
  }
  else{//we're finished with commands, so stop/zero out everything
    // COIDriveStop();
    num_commands = 0;
    command_cursor = 0;   
    goal_distance = 0; 
    distance_traveled = 0;
    angle_turned = 0;
    goal_angle = 0;
  }
}


// *******************************************************
//  *                   Execute Angle
//  ******************************************************
// // Let Katie spin:116
// until it has turn certain angle
// // Input:  Angle
// // Output: if Katie has turn that angle
// bool execute_turn(int _goal_angle){
//   //Serial.println(angle_turned);
//   if(angle_turned == 0){//if we haven't started turning
    
//     goal_angle = _goal_angle;
//     COIFetchSingleSensor(ANGLE_SINCE_LAST);
//     //  start_turn();
//     singNumber(1);
//     turnDegree(_goal_angle);
//     angle_turned = 1;
//   }

//   //eill not work
//   update_angle(200);
  
  
//   //will not work since in script...
//   if(finished_angle()){
//     COIDriveStop(); //stop
//     reset_angle();
//     return true;
//   }

//   return false;
// }

// //turns left or right depending on the goal angle
// void start_turn(){
//   if(goal_angle > 0)      COIDrive(-100, 0);
//   else if(goal_angle < 0) COIDrive( 100, 0);
//   else                               return;
// }

// // Reset the goal and turn angle
// void reset_angle(){
//   goal_angle   = 0;
//   angle_turned = 0;
// }

// // Update angle_traveled in certain time
// void update_angle(int update_time){
//   if (millis() - prev_query_time > update_time){
//     prev_query_time = millis();
//     angle_turned += COIFetchSingleSensor(ANGLE_SINCE_LAST); //millimeters to centimeters
//     //singNumber(1);
//   }
// }

// // Return true if goal angle is acheived
// bool finished_angle(){
//   if(abs(angle_turned) >= abs(goal_angle)) 
//     return  true;
//   else return false;
// }
