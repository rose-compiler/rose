#ifndef ROSE_BinaryAnalysis_Concolic_LinuxI386Executor_H
#define  ROSE_BinaryAnalysis_Concolic_LinuxI386Executor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Concrete execution for Linux ELF i386 executables. */
class LinuxI386Executor: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = LinuxI386ExecutorPtr;

private:
    DatabasePtr db_;
    TestCaseId testCaseId_;
    TestCasePtr testCase_;
    DebuggerPtr debugger_;
    ExecutionEvent::Location curLocation_;
    std::vector<uint64_t> eventKeyFrames_;              // primary keys of events that still need to be processed, in reverse order
    rose_addr_t scratchVa_ = 0;                         // subordinate address for scratch page

protected:
    LinuxI386Executor(const DatabasePtr&, TestCaseId, const TestCasePtr&);
public:
    ~LinuxI386Executor();

public:
    /** Allocating constructor for test case.
     *
     *  Constructs a new executor for the specified test case.
     *
     * @{ */
    static Ptr instance(const DatabasePtr&, TestCaseId);
    static Ptr instance(const DatabasePtr&, const TestCasePtr&);
    /** @} */

    /** Load the specimen and immediately pause.
     *
     *  Loads the specimen into memory. The executable file is created in the specified directory and named "a.out". */
    void load(const boost::filesystem::path &targetDirectory);

    /** Run to address unmonitored and unhindered.
     *
     *  The subordinate process is allowed to run unhindered and unmonitored until it reaches the specified execution address.
     *  This is one way to execute the dynamic linker. */
    void runUnhinderedTo(rose_addr_t stopVa);

    /** Run until all remaining events are processed.
     *
     *  Runs the program from the current location until the last available event has been processed, then stops. This is a
     *  no-op if there are no events. */
    void processAllEvents();

    /** Process all events for the current execution location. */
    void processEvents();

    /** Unmap nearly all memory.
     *
     *  The scratch page is not unmapped, nor is the VDSO or VVAR. */
    void unmapMemory();

    /** Save the specimen memory in the database.
     *
     *  Saves all the readable memory in the database as a sequence of events that would initialize
     *  the memory. */
    void saveMemory();

    /** Hash memory and save the hash digests in the database.
     *
     *  Hashes all available memory and saves events in the database. Replaying the event will hash memory and compare
     *  with the saved value. */
    void hashMemory();

    /** Save all registers.
     *
     *  Saves all registers in the database as an event that will restore the registers to these values. */
    void saveRegisters();

    /** Return a new event locator.
     *
     *  The returned event has the same primary key as the previous event but the secondary key is incremented. */
    ExecutionEvent::Location newLocation();

    /** Current execution address. */
    rose_addr_t ip() const;

    /** Run to the last saved event.
     *
     *  Run the specimen until just after we process the last event from the database. */
    void runToLastEvent();

private:
    // Maps a scratch page for internal use and updates scratchVa_ with the address of the page.
    void mapScratchPage();

    // Increment primary location, and set secondary location to zero. See also, newLocation.
    void incrementLocation();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
