#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusManager_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusManager_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ExecutionManager.h>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

/** Concolic testing of Linux executables.
 *
 *  Tests a single Linux executable. The concrete ranking metric is only whether the executable exited with zero status or
 *  not. */
class ExitStatusManager: public Concolic::ExecutionManager {
    using Super = Concolic::ExecutionManager;

public:
    /** Reference counting pointer. */
    using Ptr = ExitStatusManagerPtr;

protected:
    explicit ExitStatusManager(const DatabasePtr &db);

public:
    ~ExitStatusManager();

    /** Start a new round of concolic testing.
     *
     *  Creates a new database that will hold a single new test suite for the specified executable. The single seeding test
     *  case invokes the executable with the specified arguments. The actual run is not commenced until @ref run is called. */
    static Ptr create(const std::string &databaseUrl, const boost::filesystem::path &executableName,
                      const std::vector<std::string> &arguments);

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
} // namespace

#endif
#endif
