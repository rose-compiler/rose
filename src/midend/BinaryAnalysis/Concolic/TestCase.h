#ifndef ROSE_BinaryAnalysis_Concolic_TestCase_H
#define ROSE_BinaryAnalysis_Concolic_TestCase_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Sawyer/Optional.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Environment variable and its value. */
typedef std::pair<std::string /*name*/, std::string /*value*/> EnvValue;

/** Information about how to run a specimen.
 *
 *  This object points to a specimen and also contains all the information necessary to run the specimen. */
class TestCase: public Sawyer::SharedObject, public Sawyer::SharedFromThis<TestCase> {
public:
    /** Reference counting pointer to a @ref TestCase. */
    typedef Sawyer::SharedPointer<TestCase> Ptr;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::string name_;                                  // name for debugging
    std::string timestamp_;                             // time of creation
    std::string executor_;                              // name of execution environment
    SpecimenPtr specimen_;                              // the thing to run
    std::vector<std::string> args_;                     // command line arguments
    std::vector<EnvValue> env_;                         // environment variables
    Sawyer::Optional<size_t> concolicResult_;           // non-empty if concolically tested
    Sawyer::Optional<double> concreteRank_;             // rank after testing
    bool concreteIsInteresting_;                        // concrete results present and interesting?

protected:
    TestCase();

public:
    ~TestCase();

    /** Allocating default constructor. */
    static Ptr instance() {
        return Ptr(new TestCase);
    }

    /** Allocating constructor. */
    static Ptr instance(const SpecimenPtr &specimen);

    /** Property: Test case name.
     *
     *  This should be a printable name, such as a specimen name and/or serial number. It's used mostly for informational
     *  purposes such as debugging. There is no requirement that the name be unique or non-empty.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    std::string name() const;                           // value return is intentional for thread safety
    void name(const std::string&);
    /** @} */

    /** Returns printable name of test case for diagnostic output.
     *
     *  Returns a string suitable for printing to a terminal, containing the words "test case", the database ID if appropriate,
     *  and the test case name using C-style double-quoted string literal syntax if not empty.  The database ID is shown if a
     *  non-null database is specified and this test case exists in that database. */
    std::string printableName(const DatabasePtr &db = DatabasePtr());

    /** Property: Database creation timestamp string.
     *
     *  Time stamp string describing when this object was created in the database, initialized the first time the object is
     *  written to the database. If a value is assigned prior to writing to the database, then the assigned value is used
     *  instead. The value is typically specified in ISO-8601 format (except a space is used to separate the date and time for
     *  better readability, as in RFC 3339). This allows dates to be sorted chronologically as strings.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    std::string timestamp() const;
    void timestamp(const std::string&);
    /** @} */

    /** Property: Specimen.
     *
     *  The specimen that is being examined by this test case.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    SpecimenPtr specimen() const;
    void specimen(const SpecimenPtr&);
    /** @} */

    /** Command line arguments.
     *
     *  The arguments exclude <code>argv[0]</code> and <code>argv[argc]</code> and are just the elements in between.
     *
     * @{ */
    std::vector<std::string> args() const;
    void args(std::vector<std::string> arguments);
    /** @} */

    /** Environment variables.
     * @{ */
    std::vector<EnvValue> env() const;
    void env(std::vector<EnvValue> envvars);
    /** @} */

    /** Property: Result of concolic testing.
     *
     *  At this time, the result is meaningless and only used to indicate whether concolic testing has been performed.
     *
     * @{ */
    Sawyer::Optional<size_t> concolicResult() const {
        return concolicResult_;
    }
    void concolicResult(const Sawyer::Optional<size_t> &x) {
        concolicResult_ = x;
    }
    /** @} */

    /** Predicate testing whether this test case has been run concolically. */
    bool hasConcolicTest() const;

    /** Property: Sorting rank resulting from concrete test.
     *
     *  This property is also updated whenever concrete results are attached to this object.
     *
     * @{ */
    Sawyer::Optional<double> concreteRank() const;
    void concreteRank(Sawyer::Optional<double> val);
    /** @} */

    /** Property: Whether concrete results are interesting.
     *
     *  True if concrete results are present and interesting. False if not present or not interesting. Results are generally
     *  not interesting if they're the same as some other test case.
     *
     *  This property is also updated whenever concrete results are attached to this object.
     *
     * @{ */
    bool concreteIsInteresting() const;
    void concreteIsInteresting(bool);
    /** @} */

    /** Predicate testing whether this test case has been run concretely.
     *
     *  Returns true if the test case has any results. */
    bool hasConcreteTest() const;

    /** Property: Name of execution environment.
     *
     * @{ */
    const std::string& executor() const {
        return executor_;
    }
    void executor(const std::string &x) {
        executor_ = x;
    }
    /** @} */

    // We'll need to add additional information about how to run the specimen:
    //   3. Auxilliary vector (auxv)
    //   4. System calls that provide input (e.g., virtual file system, network, etc.)
    // Some of this information might need to be separated into subclasses that support different architectures since the
    // info for testing a Linux ELF executable is different from a Windows PE executable, which is different from how one
    // would run firmware.
};

} // namespace
} // namespace
} // namespace

#endif
#endif
