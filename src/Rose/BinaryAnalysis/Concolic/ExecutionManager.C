#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ExecutionManager.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>
#include <Rose/StringUtility/StringToNumber.h>
#include <ROSE_UNUSED.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ExecutionManager::~ExecutionManager() {}

ExecutionManager::ExecutionManager(const Database::Ptr &db)
    : database_(db) {
    ASSERT_not_null(db);
}

ExecutionManager::ExecutionManager(const Database::Ptr &db, const ConcolicExecutorSettings &settings, const Yaml::Node &config)
    : database_(db), concolicExecutorSettings_(settings), config_(config) {}

ExecutionManager::Ptr
ExecutionManager::instance(const ConcolicExecutorSettings &settings, const Yaml::Node &config) {
    ASSERT_require(config);
    ASSERT_require(config.isMap());
    if (!config["database"].isScalar())
        throw Exception("no database specified in YAML file");
    if (!config["architecture"].isScalar())
        throw Exception("no architecture specified in YAML file");
    if (!config["concrete"].isScalar())
        throw Exception("no concrete executor specified in YAML file");
    if (config["test-suite"] && !config["test-suite"].isScalar())
        throw Exception("test-suite must be a name when provided");

    const std::string databaseUrl = config["database"].as<std::string>();
    const auto db = Database::instance(databaseUrl);

    const std::string testSuiteName = config["test-suite"].as<std::string>();
    if (!testSuiteName.empty()) {
        TestSuite::Ptr ts = db->findTestSuite(testSuiteName);
        if (!ts)
            throw Exception("cannot find test suite \"" + StringUtility::cEscape(testSuiteName) + "\"");
        db->testSuite(ts);
    }

    return Ptr(new ExecutionManager(db, settings, config));
}

ExecutionManager::Ptr
ExecutionManager::open(const std::string &databaseUrl) {
    auto db = Database::instance(databaseUrl);
    return Ptr(new ExecutionManager(db));
}

ExecutionManager::Ptr
ExecutionManager::create(const std::string &databaseUrl, const boost::filesystem::path &executableName,
                         const std::vector<std::string> &arguments, const std::string &testCaseName) {
    ROSE_UNUSED(arguments);
    ROSE_UNUSED(testCaseName);

    const std::string executableBaseName = executableName.filename().string();
    auto specimen = Specimen::instance(executableName);

    auto testCase0 = TestCase::instance(specimen);
    testCase0->name(executableBaseName + " #0");

    auto db = Database::create(databaseUrl, executableBaseName);
    db->save(testCase0);

    return Ptr(new ExecutionManager(db));
}

const ConcolicExecutorSettings&
ExecutionManager::concolicExecutorSettings() const {
    return concolicExecutorSettings_;
}

ConcolicExecutorSettings&
ExecutionManager::concolicExecutorSettings() {
    return concolicExecutorSettings_;
}

void
ExecutionManager::concolicExecutorSettings(const ConcolicExecutorSettings &s) {
    concolicExecutorSettings_ = s;
}

Database::Ptr
ExecutionManager::database() const {
    return database_;
}

std::vector<TestSuiteId>
ExecutionManager::testSuites() const {
    ASSERT_not_null(database_);
    return database_->testSuites();
}

TestSuite::Ptr
ExecutionManager::testSuite() const {
    ASSERT_not_null(database_);
    return database_->testSuite();
}

void
ExecutionManager::testSuite(const TestSuite::Ptr &ts) {
    ASSERT_not_null(database_);
    database_->testSuite(ts);
}

std::vector<TestCaseId>
ExecutionManager::pendingConcreteResults(size_t n) {
    ASSERT_not_null(database_);
    return database_->needConcreteTesting(n);
}

TestCaseId
ExecutionManager::pendingConcreteResult() {
    for (auto tcid: pendingConcreteResults(1))
        return tcid;
    return {};
}

void
ExecutionManager::insertConcreteResults(const TestCase::Ptr &testCase, const ConcreteResult::Ptr &details) {
    testCase->concreteRank(details->rank());
    database_->saveConcreteResult(testCase, details);
}

std::vector<TestCaseId>
ExecutionManager::pendingConcolicResults(size_t n) {
    ASSERT_not_null(database_);
    return database_->needConcolicTesting(n);
}

TestCaseId
ExecutionManager::pendingConcolicResult() {
    for (auto tcid: pendingConcolicResults(1))
        return tcid;
    return {};
}

void
ExecutionManager::insertConcolicResults(const TestCase::Ptr& original, const std::vector<TestCase::Ptr> &newCases) {
    ASSERT_not_null(database_);
    original->concolicResult(1);
    database_->id(original, Update::YES);
    for (const TestCase::Ptr &tc: newCases)
        database_->save(tc);
}

bool
ExecutionManager::isFinished() const {
    ASSERT_not_null(database_);
    return !database_->hasUntested();
}

void
ExecutionManager::run() {
    ASSERT_not_null(database_);
    ASSERT_require(config_);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Configuration -- these are documented and checked in `instance`
    const std::string architectureName = config_["architecture"].as<std::string>();
    const std::string concreteName = architectureName + "::" + config_["concrete"].as<std::string>();
    const size_t concolicStride =
        std::max(StringUtility::toNumber<size_t>(config_["concolic-stride"].as<std::string>()).ok().orElse(1),
                 size_t{1});

    SAWYER_MESG(debug) <<"execution manager running\n"
                       <<"  architecture = \"" <<StringUtility::cEscape(architectureName) <<"\"\n"
                       <<"  concrete = \"" <<StringUtility::cEscape(concreteName) <<"\"\n";

    while (!isFinished()) {
        // Run as many test cases concretely as possible.
        while (TestCaseId testCaseId = pendingConcreteResult()) {
            TestCase::Ptr testCase = database()->object(testCaseId);
            std::cerr <<testCase->printableName(database()) <<" running concretely...\n";
            auto concreteExecutor = ConcreteExecutor::forge(database_, concreteName);
            if (!concreteExecutor)
                throw Exception("cannot instantiate concrete executor \"" + StringUtility::cEscape(concreteName) + "\"");
            auto concreteResult = concreteExecutor->execute(testCase);
            ASSERT_not_null(concreteResult);
            insertConcreteResults(testCase, concreteResult);
        }

        // Now that all the test cases have run concretely, run a few of the "best" ones concolically.  The "best" is defined
        // either by the ranks returned from the concrete executor, or by this class overriding pendingConcolicResult (which we
        // haven't done).
        for (TestCaseId testCaseId: pendingConcolicResults(concolicStride)) {
            TestCase::Ptr testCase = database()->object(testCaseId);
            std::cerr <<testCase->printableName(database()) <<" running concolically...\n";
            auto concolicExecutor = ConcolicExecutor::instance();
            concolicExecutor->settings(concolicExecutorSettings_);
            concolicExecutor->configureExecution(database_, testCase, config_);
            std::vector<TestCase::Ptr> newTestCases = concolicExecutor->execute();
            insertConcolicResults(testCase, newTestCases);
        }
    }
}

} // namespace
} // namespace
} // namespace

#endif
