#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryMagic.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/config.hpp>
#include <Diagnostics.h>
#include <FileSystem.h>

#ifdef ROSE_HAVE_LIBMAGIC
#include <magic.h>                                      // part of libmagic
#else
typedef int magic_t;
#endif

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

// details are defined in this .C files so users don't end up including <magic.h> into the global namespace.
class MagicNumberDetails {
public:
    magic_t cookie;
    MagicNumberDetails(magic_t &cookie): cookie(cookie) {}
    ~MagicNumberDetails() {
#ifdef ROSE_HAVE_LIBMAGIC
        if (cookie)
            magic_close(cookie);
#endif
    }
};

void
MagicNumber::init() {
#ifdef ROSE_HAVE_LIBMAGIC
    mechanism_ = FAST;
    magic_t cookie = magic_open(MAGIC_RAW);
    if (!cookie)
        throw std::runtime_error(std::string("magic_open failed: ") + strerror(errno));
    if (-1 == magic_load(cookie, NULL/*dflt files*/))
        throw std::runtime_error("magic_load failed");
    details_ = new MagicNumberDetails(cookie);
#elif defined(BOOST_WINDOWS)
    mechanism_ = NONE;
#else
    mechanism_ = SLOW;
#endif
}

MagicNumber::~MagicNumber() {
    delete details_;
}

std::string
MagicNumber::identify(const MemoryMap::Ptr &map, rose_addr_t va) const {
    uint8_t buf[256];
    size_t nBytes = map->at(va).limit(std::min(maxBytes_, sizeof buf)).read(buf).size();
    if (0==nBytes)
        return "empty";
#ifdef ROSE_HAVE_LIBMAGIC
    return magic_buffer(details_->cookie, buf, nBytes);
#elif defined(BOOST_WINDOWS)
    throw std::runtime_error("magic number identification is not supported on Microsoft Windows");
#elif BOOST_FILESYSTEM_VERSION == 2
    throw std::runtime_error("MagicNumber::identify must have either libmagic or boost::filesystem version 3");
#else
    // We can maybe still do it, but this will be much, much slower.  We copy some specimen memory into a temporary file, then
    // run the unix file(1) command on it, then delete the temp file.
    static int ncalls = 0;
    if (1 == ++ncalls)
        mlog[WARN] <<"libmagic is not available on this system; using slow method instead\n";
    FileSystem::Path tmpFile = boost::filesystem::unique_path("/tmp/ROSE-%%%%-%%%%-%%%%-%%%%");
    std::ofstream(tmpFile.c_str()).write((const char*)buf, nBytes);
    std::string cmd = "file " + tmpFile.string();
    std::string magic;
    if (FILE *f = popen(cmd.c_str(), "r")) {
        char line[1024];
        if (fgets(line, sizeof line, f))
            magic = boost::trim_right_copy(std::string(line).substr(tmpFile.string().size()+2)); // filename + ": "
        pclose(f);
    } else {
        boost::filesystem::remove(tmpFile);
        throw std::runtime_error("command file: " + tmpFile.string());
    }
    boost::filesystem::remove(tmpFile);
    return magic;
#endif
}

} // namespace
} // namespace

#endif
