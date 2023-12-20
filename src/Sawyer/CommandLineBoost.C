// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/CommandLineBoost.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

namespace Sawyer {
namespace CommandLine {
namespace Boost {

static bool
isLongName(const std::string &switchName) {
    return switchName.size() > 1;
}

static bool
isShortName(const std::string &switchName) {
    return switchName.size() == 1;
}


Switch
createSwitch(const std::string &switchNames, const std::string &doc) {
    // Split comma-separated list of switch names into long names and short names.
    std::vector<std::string> longNames;
    boost::split(longNames, switchNames, boost::is_any_of(","));
    std::vector<std::string> shortNames = longNames;
    longNames.erase(std::remove_if(longNames.begin(), longNames.end(), isShortName), longNames.end());
    shortNames.erase(std::remove_if(shortNames.begin(), shortNames.end(), isLongName), shortNames.end());

    std::vector<std::string>::iterator longIter = longNames.begin();
    std::vector<std::string>::iterator shortIter = shortNames.begin();

    // Use the first long or short name to create the switch
    ASSERT_require(!longNames.empty() || !shortNames.empty());
    Switch retval = longNames.empty() ? Switch("", (*shortIter++)[0]) : Switch(*longIter++);

    // Add the remaining long and short names
    while (longIter != longNames.end())
        retval.longName(*longIter++);
    while (shortIter != shortNames.end())
        retval.shortName((*shortIter++)[0]);

    retval.doc(doc);
    return retval;
}

options_description&
options_description::operator()(const std::string &switchNames, const value<std::string>&, const std::string &doc) {
    sg.insert(createSwitch(switchNames, doc).argument("arg", anyParser()));
    return *this;
}

options_description&
options_description::operator()(const std::string &switchName, const value<int>&, const std::string &doc) {
    sg.insert(createSwitch(switchName, doc).argument("arg", Sawyer::CommandLine::integerParser()));
    return *this;
}

options_description&
options_description::operator()(const std::string &switchName, const value<long int>&, const std::string &doc) {
    sg.insert(createSwitch(switchName, doc).argument("arg", Sawyer::CommandLine::integerParser<long int>()));
    return *this;
}

options_description&
options_description::operator()(const std::string &switchName, const value<std::vector<int> >&, const std::string &doc) {
    sg.insert(createSwitch(switchName, doc)
              .argument("arg", Sawyer::CommandLine::listParser(Sawyer::CommandLine::integerParser()))
              .whichValue(Sawyer::CommandLine::SAVE_ALL)
              .explosiveLists(true));
    return *this;
}

options_description&
options_description::operator()(const std::string &switchName, const value< std::vector<std::string> >&, const std::string &doc) {
    sg.insert(createSwitch(switchName, doc)
              .argument("arg", Sawyer::CommandLine::listParser(Sawyer::CommandLine::anyParser()))
              .whichValue(Sawyer::CommandLine::SAVE_ALL)
              .explosiveLists(true));
    return *this;
}

options_description&
options_description::operator()(const std::string &switchName, const std::string &doc) {
    sg.insert(createSwitch(switchName, doc).intrinsicValue("yes"));
    return *this;
}

options_description&
options_description::add(const options_description &other) {
    BOOST_FOREACH (const Sawyer::CommandLine::Switch &sw, other.sg.switches())
        sg.insert(sw);
    return *this;
}

void
options_description::print() const {
    Sawyer::CommandLine::Parser temporary;
    temporary
        .purpose(sg.title())
        .doc("Synopsis", "Command usage synopsis is unknown; these are only some switches")
        .doc("Description", "This is documentation for a set of switches, not the entire program.")
        .with(sg)
        .emitDocumentationToPager();
}

std::ostream&
operator<<(std::ostream &out, const options_description &x) {
    x.print();
    return out;
}

command_line_parser&
command_line_parser::options(const options_description &options) {
    parser.with(options.sg);
    return *this;
}

command_line_parser&
command_line_parser::options(const Sawyer::CommandLine::SwitchGroup &sg) {
    parser.with(sg);
    return *this;
}

size_t
variables_map::count(const std::string &swName) const {
    return pr.have(swName);
}

parsed_values
variables_map::operator[](const std::string &swName) const {
    static const parsed_values nothing;
    if (!pr.have(swName))
        return nothing;
    return parsed_values(pr.parsed(swName));
}

void
store(const Sawyer::CommandLine::ParserResult &results, variables_map &output) {
    output.pr = results;
}

void
notify(variables_map&) {}


} // namespace
} // namespace
} // namespace
