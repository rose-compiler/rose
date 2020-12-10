#ifndef ROSE_BinaryAnalysis_Demangler_H
#define ROSE_BinaryAnalysis_Demangler_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {

class Demangler {
public:
    typedef Sawyer::Container::Map<std::string /*mangled*/, std::string /*non-mangled*/> NameMap;

private:
    boost::filesystem::path cxxFiltExe_;                // name or path of the c++filt command ($PATH is used to search)
    NameMap nameMap_;                                   // cache of de-mangled names
    std::string compiler_;                              // format of mangled names

public:
    /** Property: Name of c++filt program.
     *
     *  This is the name of the c++filt command that gets run to convert mangled names to demangled names. If it's not an
     *  absolute name then the normal executable search is performed (i.e., $PATH variable).
     *
     * @{ */
    const boost::filesystem::path& cxxFiltExe() const { return cxxFiltExe_; }
    void cxxFiltExe(const boost::filesystem::path &p) { cxxFiltExe_ = p; }
    /** @} */

    /** Property: Format of mangled names.
     *
     *  Each compiler has slightly different rules for how names are mangled. This property controls the format and should be
     *  one of the following strings:
     *
     *  @li "auto" (or empty string): automatic selection based on c++filt executable (this is the default).
     *  @li "gnu": format used by GNU C++ compiler (g++).
     *  @li "lucid": format used by the Lucid compiler (lcc).
     *  @li "arm": format used by the C++ Annotated Reference Manual.
     *  @li "hp": format used by the HP compiler (aCC).
     *  @li "edg": format used by EDG-based compilers.
     *  @li "gnu-v3": format used by the GNU C++ compiler (g++) with the V3 ABI.
     *  @li "java": format used by the GNU Java compiler (gcj).
     *  @li "gnat": format used by the GNU Ada compiler (GNAT).
     *
     *  ROSE itself does not check these strings, so if your c++filt supports other values for its "-s" switch they will work.
     *
     * @{ */
    const std::string& compiler() const { return compiler_; }
    void compiler(const std::string &s) { compiler_ = s; }
    /** @} */

    /** Demangle lots of names.
     *
     *  The most efficient way to invoke this analyzer is to provide it with as many names as possible. It will send them all
     *  to the c++filt program (@ref cxxFiltExe property) all at once and cache the results to query later. */
    void fillCache(const std::vector<std::string> &mangledNames);

    /** Demangle one name.
     *
     *  If the name is already cached, then return the cached value. Otherwise invoke c++filt to demangle this one name and
     *  cache the result.  A name that cannot be demangled is returned in its original form.
     *
     *  It is not efficient to fill the cache one name at a time; use @ref fillCache first if possible, and then call this
     *  function to retrieve the results. */
    std::string demangle(const std::string &mangledName);

    /** Clear the cache. */
    void clear() { nameMap_.clear(); }

    /** Number of items in the cache. */
    size_t size() const { return nameMap_.size(); }

    /** Map of all cached names. */
    const NameMap& allNames() const { return nameMap_; }

    /** Insert a mangled/demangled pair.
     *
     *  Adds (or modifies) the mangled/demangled pair to the cache. */
    void insert(const std::string &mangledName, const std::string &demangledName);
};

} // namespace
} // namespace

#endif
#endif
