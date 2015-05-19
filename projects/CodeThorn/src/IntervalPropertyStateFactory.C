#include "sage3basic.h"
#include "IntervalPropertyStateFactory.h"
#include "IntervalPropertyState.h"

SPRAY::IntervalPropertyStateFactory::IntervalPropertyStateFactory() {
}

SPRAY::PropertyState* SPRAY::IntervalPropertyStateFactory::create() {
  return new IntervalPropertyState();
}

SPRAY::IntervalPropertyStateFactory::~IntervalPropertyStateFactory() {
}
