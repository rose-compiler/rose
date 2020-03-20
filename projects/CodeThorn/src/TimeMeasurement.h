#ifndef TIME_MEASUREMENT_H
#define TIME_MEASUREMENT_H

/* 

   The class TimeMeasurement performs a simple wall time measurement
   using the linux system function gettimeofday. The measurement can
   be started with 'start()' and stopped with 'stop()'. The start and
   stop operations must match exactly, otherwise the functions throw a
   std::runtime_error exception. The function getTimeDurationAndStop
   only succeeds if the time measurement is already running, otherwise
   is throws a std::runtime_error exception. By default the time
   duration is 0. The measured time duration is reported as type
   TimeDuration.

   example: TimeMeasurement tm; 
            tm.start(); ... ;
            TimeDuration d=tm.getTimeDurationAndStop();
   or:
            tm.start(); ... ;
            TimeDuration d1=tm.getRunningTimeDuration();
            TimeDuration d2=tm.getRunningTimeDuration();
            tm.stop(); 
            TimeDuration d3=tm.getTimeDuration();


   The class TimeDuration stores time in micro seconds and provides
   conversion functions to return time in milli seconds, seconds,
   minutes, and hours (double value). The function longTimeString
   provides a human readable time format. The class also provides
   overloaded operators to perform arithmetic calculations based on
   the TimeDuration type. Therefore one can convert also only the
   final result of a time calculation (e.g. when printed).

   example: TimeDuration phase1=tm1.getTimeDurationAndStop();
            TimeDuration phase2=tm2.getTimeDurationAndStop();
            std::cout<<"Measured time: "<<(d1+d2).milliSeconds()<<" ms."<<std::endl;
            std::cout<<"Measured time: "<<(d1+d2).seconds()<<" s."<<std::endl;

 */

#include <sys/time.h>
#include <string>
#include <sstream>

class TimeDuration {
 public:
  TimeDuration();
  TimeDuration(double timeDuration);
  std::string longTimeString();
  double microSeconds();
  double milliSeconds();
  double seconds();
  double minutes();
  double hours();
  TimeDuration operator+(const TimeDuration &other);
  TimeDuration operator-(const TimeDuration &other);
  TimeDuration& operator+=(const TimeDuration& rhs);
  TimeDuration& operator-=(const TimeDuration& rhs);
 private:
  double _timeDuration;
};

enum TimeMeasurementState { TIME_RUNNING, TIME_STOPPED };

class TimeMeasurement {
 public:
  TimeMeasurement();
  virtual void start();
  virtual void stop();
  virtual TimeDuration getTimeDuration(); // must be stopped
  virtual TimeDuration getTimeDurationAndStop(); // must be running
  // must be running
  virtual TimeDuration getTimeDurationAndKeepRunning();
 private:
  TimeMeasurementState state;
  timeval startCount;
  timeval endCount;
};

#endif
