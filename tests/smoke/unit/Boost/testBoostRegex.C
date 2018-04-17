// Tests that boost::regex is usable within ROSE.  For instance, this test will fail miserably (segfault,
// throw std::bad_alloc, failed assertion, etc. depending on compiler flags) if the ROSE (and this file
// in particular) is compiled with -DBOOST_REGEX_MATCH_EXTRA but libboost_regex.so is compiled without
// any special features.
#include <boost/regex.hpp>

int main() {
    const char *s = "[;,]\\s*";
    const char *t = "one, two";
    boost::regex re(s);
    boost::cmatch matched;
    if (regex_search(t, matched, re) && 3==matched.position())
        return 0;
    return 1;
}
