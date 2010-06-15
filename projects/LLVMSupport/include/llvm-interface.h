/* Header file for LLVM interface */

#ifndef _LLVM_INTERFACE_H
#define _LLVM_INTERFACE_H

#include <vector>
#include <map>
#include <cassert>
#include <string>
#include "llvm/ADT/IndexedMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/CallingConv.h"
#include "llvm/Intrinsics.h"
#include "llvm/System/DataTypes.h"
#include "llvm/Support/IRBuilder.h"

namespace llvm
{
    class Module;
    class GlobalVariable;
    class Function;
    class GlobalValue;
    class BasicBlock;
    class Instruction; 
    class AllocaInst;
    class Value;
    class Constant;
    class Type;
    class FunctionType;
}

#endif
