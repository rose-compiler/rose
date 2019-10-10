#include "TimeMeasurement.h"
#include <stdexcept>
#include <cmath>     
#include <iomanip>

TimeDuration::TimeDuration():_timeDuration(0.0) {
}

TimeDuration::TimeDuration(double timeDuration):_timeDuration(timeDuration) {
}

TimeDuration TimeDuration::operator+(const TimeDuration &other) {
  return TimeDuration(_timeDuration+other._timeDuration);
}

TimeDuration& TimeDuration::operator+=(const TimeDuration& rhs){
      this->_timeDuration += rhs._timeDuration;
      return *this;
}

double TimeDuration::microSeconds() {
  return _timeDuration;
}

double TimeDuration::milliSeconds() {
  return _timeDuration*0.001;
}   

double TimeDuration::seconds() {
  return _timeDuration* 0.000001;
}

double TimeDuration::minutes() {
  return seconds()/60.0;
}

double TimeDuration::hours() {
  return minutes()/60.0;
}

std::string TimeDuration::longTimeString() {
  int ms=std::floor(milliSeconds());
  int s=std::floor(seconds());
  int m=std::floor(minutes());
  int h=std::floor(hours());
  std::stringstream ss;
  ss<<std::setfill('0')<<std::setw(2)<<h<<":"<<m<<":"<<s<<"."<<std::setw(3)<<ms;
  return ss.str();
}

TimeMeasurement::TimeMeasurement():
  state(TIME_STOPPED),
  startTimeInMicroSeconds(0),
  endTimeInMicroSeconds(0)
{
    startCount.tv_sec = startCount.tv_usec = 0;
    endCount.tv_sec = endCount.tv_usec = 0;
}

void TimeMeasurement::start() {
  //  if(state==TIME_RUNNING) {
  //  throw std::runtime_error("Internal error 1: TimeMeasurement in wrong state (RUNNING).");
  //} else {
    state=TIME_RUNNING;
    gettimeofday(&startCount, 0);
    //}
}

void TimeMeasurement::stop() {
  if(state==TIME_STOPPED) {
    throw std::runtime_error("Internal error 2: TimeMeasurement in wrong state (STOPPED).");
  } else {
    gettimeofday(&endCount, NULL);
    state=TIME_STOPPED;
  }  
} 

TimeDuration TimeMeasurement::getTimeDuration() {
  //  if(state==TIME_RUNNING) {
  //  throw std::runtime_error("Internal error 3: TimeMeasurement in wrong state (RUNNING).");
  //} else {
    return TimeDuration((endCount.tv_sec-startCount.tv_sec)*1000000.0+(endCount.tv_usec-startCount.tv_usec));
    //}
}
