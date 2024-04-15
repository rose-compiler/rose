#ifndef ROSE_BinaryAnalysis_ModelChecker_SourceLister_H
#define ROSE_BinaryAnalysis_ModelChecker_SourceLister_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/SourceLocation.h>

#include <Sawyer/LineVector.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Lists parts of source files.
 *
 *  A source lister lists parts of source files and caches the results. */
class SourceLister {
public:
    using Ptr = SourceListerPtr;

    struct Settings {
        std::vector<boost::filesystem::path> directories;
        boost::format currentLineFormat{"|       ==> %06d  %s"};
        boost::format contextLineFormat{"|           %06d  %s"};
        size_t leadingContext = 3;
        size_t trailingContext = 3;
    };

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    Sawyer::Container::Map<boost::filesystem::path, std::shared_ptr<Sawyer::Container::LineVector>> cache_;
    Settings settings_;

protected:
    SourceLister();
    explicit SourceLister(const Settings&);

public:
    /** Allocating constructor.
     *
     *  Initial settings can be specified with an argument, or the settings can be changed after construction through
     *  the various properties.
     *
     *  Thread safety: This constructor is thread safe.
     *
     * @{ */
    static Ptr instance();
    static Ptr instance(const Settings&);
    /** @} */

    /** Property: Format string for current line.
     *
     *  This property is a @c boost::format string for the current line. It should expect two arguments: a @p size_t line
     *  number and the line content as an @c std::string.
     *
     *  Thread safety: This method is thread safe.
     *
     *  @{ */
    boost::format currentLineFormat() const;
    void currentLineFormat(const boost::format&);
    /** @} */

    /** Property: Format context lines.
     *
     *  This property is a @c boost::format string for the lines that appear before and after the current line. It should
     *  expect two arguments: a @p size_t line number and the @c std::string line content.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    boost::format contextLineFormat() const;
    void contextLineFormat(const boost::format&);
    /** @} */

    /** Property: Number of leading context lines.
     *
     *  This is the number of lines that will be printed before the specified line. It is not an error if the leading context
     *  lines don't exist.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    size_t leadingContext() const;
    void leadingContext(size_t);
    /** @} */

    /** Property: Number of trailing context lines.
     *
     *  This is the number of lines that will be printed after the specified line. It is not an error if the trailing context
     *  lines don't exist.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    size_t trailingContext() const;
    void trailingContext(size_t);
    /** @} */

    /** Property: Search directories.
     *
     *  This property is a vector of directory names that will be searched in the order specified.
     *
     *  Thread safety: This method is thread safe. See also @ref appendDirectory for a more efficient
     *  way to change this property.
     *
     *  @{ */
    std::vector<boost::filesystem::path> directories() const;
    void directories(const std::vector<boost::filesystem::path>&);
    /** @} */

    /** Add a search directory.
     *
     *  Adds the specified directory to the end of the search path.  This is faster than obtaining a copy of the property's
     *  value, appending the directory, and then saving the property. Also, it's an atomic operation.
     *
     *  Thread safety: This method is thread safe. */
    void appendDirectory(const boost::filesystem::path&);

    /** List some lines of source code.
     *
     *  Tries to find the specified source location and then lists it. Returns true if lines were listed,
     *  or false if not listed. Failures are due to files not being found and/or the file not containing the
     *  the requested line number. The results are cached.
     *
     *  Thread safety: This method is thread safe. */
    bool list(std::ostream&, const SourceLocation&, const std::string &prefix);

private:
    // Decompose a path into search components. For instance, the path "/foo/bar/baz.c" will return
    //   /foo/bar/baz.c
    //   foo/bar/baz.c
    //   bar/baz.c
    //   baz.c
    static std::vector<boost::filesystem::path> decompose(const boost::filesystem::path&);

    // Find a readable file by searching in the specified directory for each of the file names. Returns
    // the path found, or empty.
    static boost::filesystem::path findFile(const boost::filesystem::path &dir, const std::vector<boost::filesystem::path> &names);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
