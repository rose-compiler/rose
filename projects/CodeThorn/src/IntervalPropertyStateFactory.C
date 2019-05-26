#include "sage3basic.h"
#include "IntervalPropertyStateFactory.h"
#include "IntervalPropertyState.h"

CodeThorn::IntervalPropertyStateFactory::IntervalPropertyStateFactory() {
}

CodeThorn::PropertyState* CodeThorn::IntervalPropertyStateFactory::create() {
  return new IntervalPropertyState();
}

CodeThorn::IntervalPropertyStateFactory::~IntervalPropertyStateFactory() {
}
