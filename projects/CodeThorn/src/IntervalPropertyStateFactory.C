#include "sage3basic.h"
#include "IntervalPropertyStateFactory.h"
#include "IntervalPropertyState.h"

IntervalPropertyStateFactory::IntervalPropertyStateFactory() {
}

PropertyState* IntervalPropertyStateFactory::create() {
  return new IntervalPropertyState();
}

IntervalPropertyStateFactory::~IntervalPropertyStateFactory() {
}
