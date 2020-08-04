#ifndef CODETHORN_LIB_H
#define CODETHORN_LIB_H

#include "Diagnostics.h"

namespace CodeThorn {
  void initDiagnostics();
  extern Sawyer::Message::Facility logger;
  void turnOffRoseWarnings();
}

#endif
