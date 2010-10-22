#ifndef ROSE_POWERPCINSTRUCTIONPROPERTIES_H
#define ROSE_POWERPCINSTRUCTIONPROPERTIES_H

#include <vector>
#include <stdint.h>

class SgAsmPowerpcInstruction;

const char* sprToString(PowerpcSpecialPurposeRegister n);
const char* tbrToString(PowerpcTimeBaseRegister n);

#endif // ROSE_POWERPCINSTRUCTIONPROPERTIES_H
