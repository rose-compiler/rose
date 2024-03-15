#ifndef ROSE_BinaryAnalysis_Debugger_BasicTypes_H
#define ROSE_BinaryAnalysis_Debugger_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

#include <Rose/BitFlags.h>
#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

class Base;
using BasePtr = Sawyer::SharedPointer<Base>;
using Ptr = BasePtr;

class Exception;

class Gdb;
using GdbPtr = Sawyer::SharedPointer<Gdb>;

class GdbResponse;

class Linux;
using LinuxPtr = Sawyer::SharedPointer<Linux>;

class ThreadId;

/** Diagnostic facility for debuggers. */
extern Sawyer::Message::Facility mlog;

/** Initialize diagnostic facility.
 *
 *  This is called automatically when the ROSE library is initialized. */
void initDiagnostics();

/** Action for trace filter callback. */
enum class FilterActionFlag {
    REJECT = 0x00000001,                          /**< Reject the current address, not appending it to the trace. */
    STOP   = 0x00000002                           /**< Abort tracing, either appending or rejecting the current address. */
};

/** Return value for tracing.
 *
 *  The return value from the trace filter indicates whether the current address should be appended to the trace or
 *  rejected, and whether the tracing operation should continue or stop.  A default constructed @ref FilterAction will
 *  append the current address to the trace and continue tracing, which is normally what one wants. */
using FilterAction = BitFlags<FilterActionFlag>;

} // namespace
} // namespace
} // namespace

#endif
#endif
