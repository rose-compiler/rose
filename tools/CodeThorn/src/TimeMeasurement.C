#include "TimeMeasurement.h"
#include <stdexcept>
#include <cmath>     
#include <iomanip>
#include <iostream>

using namespace std;

TimeDuration::TimeDuration():_timeDuration(0.0) {
}

TimeDuration::TimeDuration(double timeDuration):_timeDuration(timeDuration) {
}

TimeDuration TimeDuration::operator+(const TimeDuration &other) {
  return TimeDuration(_timeDuration+other._timeDuration);
}

TimeDuration TimeDuration::operator-(const TimeDuration &other) {
  return TimeDuration(_timeDuration-other._timeDuration);
}

TimeDuration& TimeDuration::operator+=(const TimeDuration& rhs){
      this->_timeDuration += rhs._timeDuration;
      return *this;
}

TimeDuration& TimeDuration::operator-=(const TimeDuration& rhs){
      this->_timeDuration -= rhs._timeDuration;
      return *this;
}

double TimeDuration::microSeconds() {
  return _timeDuration;
}

double TimeDuration::milliSeconds() {
  return _timeDuration/1000.0;
}   

double TimeDuration::seconds() {
  return _timeDuration/1000000.0;
}

double TimeDuration::minutes() {
  return seconds()/60.0;
}

double TimeDuration::hours() {
  return minutes()/60.0;
}

std::string TimeDuration::longTimeString() {
  int t=seconds();
  int h=t/3600;
  t=t-h*3600;
  int m=t/60;
  t=t-m*60;
  int s=t;
  int ms=milliSeconds()-(h*3600+m*60+s)*1000;
  std::stringstream ss;
  ss<<std::setfill('0')
    <<std::setw(2)<<h<<"h:"
    <<std::setw(2)<<m<<"m:"<<std::setw(2)<<s<<"."<<std::setw(3)<<ms<<"s";
  return ss.str();
}

TimeMeasurement::TimeMeasurement():
  state(TIME_STOPPED)
{
    startCount.tv_sec = startCount.tv_usec = 0;
    endCount.tv_sec = endCount.tv_usec = 0;
}

void TimeMeasurement::start() {
  if(state==TIME_RUNNING) {
    throw std::runtime_error("Time Measurement: error 1: start(): TimeMeasurement already running (state: RUNNING).");
  } else {
    state=TIME_RUNNING;
    gettimeofday(&startCount, 0);
  }
}

void TimeMeasurement::stop() {
  if(state==TIME_STOPPED) {
    throw std::runtime_error("Time Measurement: error 2: stop(): TimeMeasurement already stopped (state: STOPPED).");
  } else {
    gettimeofday(&endCount, NULL);
    state=TIME_STOPPED;
  }  
} 

void TimeMeasurement::resume() {
  if(state==TIME_RUNNING) {
    throw std::runtime_error("Time Measurement: error 6: resume(): TimeMeasurement not stopped (state: RUNNING).");
  } else {
    state=TIME_RUNNING;
  }  
} 

TimeDuration TimeMeasurement::getTimeDuration() {
  //if(state==TIME_RUNNING) {
  //throw std::runtime_error("Time Measurement: error 3: : getTimeDuration: TimeMeasurement not stopped (state: RUNNING).");
    //}
  switch(state) {
  case TIME_RUNNING: {
    stop();
    TimeDuration td=TimeDuration((endCount.tv_sec-startCount.tv_sec)*1000000.0+(endCount.tv_usec-startCount.tv_usec));
    resume();
    return td;
  }
  case TIME_STOPPED: {
    TimeDuration td=TimeDuration((endCount.tv_sec-startCount.tv_sec)*1000000.0+(endCount.tv_usec-startCount.tv_usec));
    return td;
  }
  default:
    throw std::runtime_error("Time Measurement: error 5: : unknown state (not RUNNING, not STOPPED).");
  }
}

TimeDuration TimeMeasurement::getTimeDurationAndKeepRunning() {
  if(state==TIME_STOPPED) {
    throw std::runtime_error("Time Measurement: error 4: : getTimeDurationAndKeepRunning: TimeMeasurement already stopped, cannot keep it running (state: STOPPED).");
  }
  stop();
  TimeDuration td=TimeDuration((endCount.tv_sec-startCount.tv_sec)*1000000.0+(endCount.tv_usec-startCount.tv_usec));
  resume();
  return td;
}

TimeDuration TimeMeasurement::getTimeDurationAndStop() {
  if(state==TIME_STOPPED) {
    throw std::runtime_error("Time Measurement: error 5: : getTimeDurationAndStop: TimeMeasurement already stopped, cannot stop again (state: STOPPED).");
  }
  stop();
  TimeDuration td=TimeDuration((endCount.tv_sec-startCount.tv_sec)*1000000.0+(endCount.tv_usec-startCount.tv_usec));
  return td;
}
