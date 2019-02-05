// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Message.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace Sawyer::Message;

static const Importance impLevels[] = {DEBUG, TRACE, WHERE, MARCH, INFO, ERROR, FATAL};
static const size_t nImpLevels = sizeof(impLevels) / sizeof(impLevels[0]);
static bool isLineFeed(char ch) { return '\n'==ch; }

static std::vector<std::string>
splitMessages(std::string all) {
    while (boost::ends_with(all, "\n"))
        boost::erase_tail(all, 1);
    std::vector<std::string> each;
    boost::split(each, all, isLineFeed);
    return each;
}

static bool
eachContains(const std::vector<std::string> &messages, const std::string &pattern) {
    size_t nErrors = 0;
    boost::regex re(pattern);
    for (size_t i=0; i<messages.size(); ++i) {
        if (!boost::regex_search(messages[i], re)) {
            std::cout <<"  message #" <<i <<" does not match /" <<pattern <<"/\n";
            std::cout <<"    " <<messages[i] <<"\n";
            ++nErrors;
        }
    }
    return 0==nErrors;
}

static void
showMessages(const std::vector<std::string> &messages) {
    BOOST_FOREACH (const std::string &mesg, messages)
        std::cout <<"    " <<mesg <<"\n";
}

static void
testConstructors() {
    PrefixPtr prefix = Prefix::instance();
    std::ostringstream output;
    std::ostringstream empty;

    // Main constructor
    Stream s1("testConstructors", ERROR, StreamSink::instance(output, prefix));
    s1 <<"pattern 1\n";
    ASSERT_always_require(output.str().find("pattern 1") != std::string::npos);

    // Main copy constructor
    Stream s2(s1);
    s2 <<"pattern 2\n";
    ASSERT_always_require(output.str().find("pattern 2") != std::string::npos);

    // Copy from std::ostring that is really a Sawyer::Message::Stream
    Stream s3(s1 <<"");
    s3 <<"pattern 3\n";
    ASSERT_always_require(output.str().find("pattern 3") != std::string::npos);

    // Main assignment operator
    Stream s4("nothing", ERROR, StreamSink::instance(empty, prefix));
    s4 = s1;
    s4 <<"pattern 4\n";
    ASSERT_always_require(empty.str().empty());
    ASSERT_always_require(output.str().find("pattern 4") != std::string::npos);

    // Assignment from std::ostream that's really a Sawyer::Message::Stream
    Stream s5("nothing", ERROR, StreamSink::instance(empty, prefix));
    s5 = (s1 <<"blank\n");
    s5 <<"pattern 5\n";
    ASSERT_always_require(empty.str().empty());
    ASSERT_always_require(output.str().find("pattern 5") != std::string::npos);
}

static void
controlImportanceCase() {
    std::cout <<"testing case insensitive importance names in Facilities::control...\n";

    Facilities facilities;
    Facility f1;
    f1.initialize("f1");
    facilities.insert(f1);

    static const char *impNames[] = {"none", "debug", "trace", "where", "march", "info", "error", "fatal", "all" };
    static const unsigned bits[]  = {0x00,   0x01,    0x02,    0x04,    0x08,    0x10,   0x20,    0x40,    0x7f  };

    size_t nErrors = 0;
    for (size_t pass=0; pass<2; ++pass) {
        for (size_t i=0; i<sizeof(impNames)/sizeof(impNames[0]); ++i) {
            std::string s = "f1(none," +
                            (0==pass ? std::string(impNames[i]) : boost::to_upper_copy(std::string(impNames[i]))) +
                            ")";
            std::cout <<"  facilities.control(\"" <<s <<"\")\n";
            std::string error = facilities.control(s);
            if (!error.empty()) {
                std::cout <<"    error from facilities.control: " <<error;
                ++nErrors;
            } else {
                unsigned mask = 0;
                for (size_t j=0; j<nImpLevels; ++j) {
                    if (f1[impLevels[j]])
                        mask |= 1 << j;
                }
                if (mask != bits[i]) {
                    std::cout <<"    wrong streams enabled for " <<impNames[i]
                              <<": got 0x" <<std::hex <<mask <<" but expected 0x" <<bits[i] <<"\n"
                              <<std::dec;
                    facilities.print(std::cout);
                    ++nErrors;
                }
            }
        }
    }
    if (nErrors>0) {
        std::cout <<"  FAILED!\n";
        abort();
    }
    std::cout <<"  passed.\n";
}

static void
streamRename() {
    std::cout <<"testing that stream names can be changed on the fly...\n";

    std::ostringstream ss;
    DestinationPtr destination = StreamSink::instance(ss);
    Facilities facilities;
    Facility f1("NAME1", destination);                  // NAME1 is the initial name for all streams

    f1[INFO] <<"#0 should be from NAME1\n";
    f1[INFO].facilityName("NAME2", false);              // change the name only for the next message
    f1[INFO] <<"#1 should be from NAME2\n";
    f1[INFO] <<"#2 should be from NAME1 again\n";
    f1[INFO].facilityName("NAME3");                     // changes the name for subsequent messages
    f1[INFO] <<"#3 should be from NAME3\n";
    f1[INFO] <<"#4 should be from NAME3 still\n";
    f1[WARN] <<"#5 should be original NAME1\n";         // other streams should have the original name still

    std::vector<std::string> messages = splitMessages(ss.str());
    if (messages.size() != 6) {
        std::cout <<"  wrong number of messages: got " <<messages.size() <<" but expected 5\n";
        showMessages(messages);
        std::cout <<"  FAILED!\n";
        abort();
    }

    if (!eachContains(messages, "NAME(\\d).*NAME\\1")) {
        showMessages(messages);
        std::cout <<"  FAILED!\n";
        abort();
    }
    std::cout <<"  passed.\n";
}

static void
facilityRename() {
    std::cout <<"testing that a facility can be renamed...\n";

    std::ostringstream ss;
    DestinationPtr destination = StreamSink::instance(ss);
    Facilities facilities;
    Facility f1("NAME1", destination);
    facilities.insert(f1);

    size_t n = 0;
    for (size_t i=0; i<nImpLevels; ++i) {
        f1[impLevels[i]] <<"#" <<n++ <<" should be from NAME1\n";
        f1[impLevels[i]] <<"#" <<n++ <<" should be from NAME1 still\n";
    }

    f1.renameStreams("NAME2");
    for (size_t i=0; i<nImpLevels; ++i) {
        f1[impLevels[i]] <<"#" <<n++ <<" should be from NAME2\n";
        f1[impLevels[i]] <<"#" <<n++ <<" should be from NAME2 still\n";
    }

    std::vector<std::string> messages = splitMessages(ss.str());
    if (!eachContains(messages, "NAME(\\d).*NAME\\1")) {
        showMessages(messages);
        std::cout <<"  FAILED!\n";
        abort();
    }
    std::cout <<"  passed.\n";
}

static void
facilitiesRename() {
    std::cout <<"renaming a facility vs. Facilities::control...\n";

    Facilities facilities;
    Facility f1;
    f1.initialize("f1");
    facilities.insert(f1);

    std::string error = facilities.control("f1(all)");
    if (!error.empty()) {
        std::cout <<"  " <<error;
        std::cout <<"  FAILED!\n";
        abort();
    }

    // Renaming the streams in a facility will not change how the facility itself is identified.
    f1.renameStreams("f1new");
    error = facilities.control("f1(all)");
    if (!error.empty()) {
        std::cout <<"  " <<error;
        std::cout <<"  FAILED!\n";
        abort();
    }
    std::cout <<"  passed.\n";
}

int
main() {
    initializeLibrary();
    testConstructors();
    controlImportanceCase();
    streamRename();
    facilityRename();
    facilitiesRename();
}
