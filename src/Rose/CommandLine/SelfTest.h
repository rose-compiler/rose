#ifndef ROSE_CommandLine_SelfTest_H
#define ROSE_CommandLine_SelfTest_H
#include <RoseFirst.h>

#include <rosedll.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace CommandLine {

/** Base class for self tests.
 *
 *  Each test has a name and a functor that takes no arguments.
 *
 *  See also, @ref selfTests, @ref runSelfTestsAndExit. */
class ROSE_DLL_API SelfTest: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<SelfTest> Ptr;

    /** Short name for test.
     *
     *  This can be a single word or multiple words, but short enough to easily fit on a single line of output. It should not
     *  be capitalized or punctuated since the returned string will be used in a larger diagnostic message. */
    virtual std::string name() const = 0;

    /** The actual test.
     *
     *  This is the actual test to run. It should return true if successful, false if not successful. */
    virtual bool operator()() = 0;
};

/** Collection of self tests to be run by --self-tests switch.
 *
 *  The unit tests are run sequentially from first to last, optionally stopping at the first test that fails. Null pointers are
 *  ignored without causing any failure. */
ROSE_DLL_API extern std::vector<SelfTest::Ptr> selfTests;

/** Runs the self tests and then exits the program.
 *
 *  Sequentially runs the self tests in @c selfTests and then exits. Exit status is success if and only if no test returns
 *  false. Tests that are null pointers are ignored. */
ROSE_DLL_API void runSelfTestsAndExit();

/** Convenient way to add a command-line self test. */
template<class SelfTest>
void insertSelfTest() {
    selfTests.push_back(typename SelfTest::Ptr(new SelfTest));
}

/** Run self tests from the command-line, then exit. */
class SelfTests: public Sawyer::CommandLine::SwitchAction {
protected:
    SelfTests() {}

public:
    typedef Sawyer::SharedPointer<SelfTests> Ptr;

    static Ptr instance() {
        return Ptr(new SelfTests);
    }

protected:
    void operator()(const Sawyer::CommandLine::ParserResult &cmdline) {
        ASSERT_always_require(cmdline.have("self-test"));
        runSelfTestsAndExit();
    }
};

} // namespace
} // namespace

#endif
