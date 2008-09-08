#ifndef COMPASS_GUI_TIMER_H
#define COMPASS_GUI_TIMER_H

#include <ctime>

class Timer 
{
  public:
    Timer();
    ~Timer();
    static double getTimeEllapsed();

  protected:
    static double _timeEllapsed;
    clock_t _st;
}; //class Timer

#endif
