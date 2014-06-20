#include <sawyer/Sawyer.h>
#include <sawyer/Message.h>

namespace Sawyer {

bool isInitialized;

bool initializeLibrary() {
    if (!isInitialized) {
        Message::initializeLibrary();
        isInitialized = true;
    }
    return true;
}

// Presence/absense of strtoll and strtoull depends on the compiler rather than the target environment.   For instance, MinGW
// has strtoll and strtoull and lacks _strtoi64 and _strtoui64 even though both MinGW and MVC are targeting a Windows
// environment.
boost::int64_t strtoll(const char *input, char **rest, int base) {
#ifdef _MSC_VER
    return _strtoi64(input, rest, base);
#else
    return ::strtoll(input, rest, base);
#endif
}

boost::uint64_t strtoull(const char *input, char **rest, int base) {
#ifdef _MSC_VER
    return _strtoui64(input, rest, base);
#else
    return ::strtoull(input, rest, base);
#endif
}

} // namespace
