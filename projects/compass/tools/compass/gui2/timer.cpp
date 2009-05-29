#include "timer.h"

double Timer::_timeEllapsed = 0;

Timer::Timer() {
   _timeEllapsed = 0;
   _st = clock();
}
Timer::~Timer() {
   _timeEllapsed = (double) (clock() - _st) / (double) CLOCKS_PER_SEC;
}

double Timer::getTimeEllapsed() {
   return _timeEllapsed;
}
