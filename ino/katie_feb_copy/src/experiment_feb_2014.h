// experiment_feb_2014.h


bool start_flag = false;
unsigned long start_time = 0;
unsigned long check_time = 0;


void feb_experiment_scanning() {
  int angle = 30;
  if (check_time < 20000) {
    check_time = millis() - start_time;
  } else {
      turnDegree(angle);
      // generate time complete estimate
      for ( int t = 0; t < 50; t++) {
      	delay(5);
      	if (stream)
    		stream_data();
      }
      start_time = millis();
      check_time = 0;
    }
  }




