#ifndef ROSE_BinaryAnalysis_Concolic_M68kExitStatus_H
#define ROSE_BinaryAnalysis_Concolic_M68kExitStatus_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ExecutionManager.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Concolic testing of Motorola 68k bare systems.
 *
 *  Tests a single m68k ELF executable running on a bare machine. The concrete ranking metric is only whether the executable
 *  exited with zero status or not. */
class M68kExitStatus: public ExecutionManager {
    /** Reference counting pointer to @ref M68kExitStatus. */
    using Ptr = M68kExitStatusPtr;

protected:
    explicit M68kExitStatus(const DatabasePtr &db);

public:
    ~M68kExitStatus();

    /** Start a new round of concolic testing.
     *
     *  Create a new database that will hold a single new test suite for the specified executable. The single seeding test case
     *  invokes the executable with the specified arguments. The actual run is not commenced until @ref run is called. */
    static Ptr create(const std::string &databaseUrl, const boost::filesystem::path &executableName);

    /** Resume concolic testing using the specified database.
     *
     *  If a test suite name is specified then it must exist in the database. If no test suite name is specified then the
     *  database must contain exactly one test suite which is the one that will be used. The actual run is not commenced until
     *  @ref run is called. */
    static Ptr instance(const std::string &databaseUri, const std::string &testSuiteName = "");

    virtual void run() override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
