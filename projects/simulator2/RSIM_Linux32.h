#ifndef ROSE_RSIM_Linux32_H
#define ROSE_RSIM_Linux32_H

#include "RSIM_Simulator.h"

/** Simulator for 32-bit Linux guests. */
class RSIM_Linux32: public RSIM_Simulator {
public:
    RSIM_Linux32() {
        ctor();
    }
private:
    void ctor();
};

#endif /* ROSE_RSIM_Linux32_H */
