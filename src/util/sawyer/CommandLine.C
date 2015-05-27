// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#include <sawyer/CommandLine.h>

#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/config.hpp>
#include <boost/foreach.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/regex.hpp>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sawyer/Assert.h>
#include <sawyer/MarkupPod.h>
#include <sawyer/Message.h>
#include <sawyer/Optional.h>
#include <sawyer/Sawyer.h>
#include <set>
#include <sstream>

#ifndef BOOST_WINDOWS
#if (__APPLE__ && __MACH__)
#include <termios.h>
#else
#include <termio.h>
#endif
#include <sys/ioctl.h>
#endif

namespace Sawyer {
namespace CommandLine {

const std::string STR_NONE(" %-NONE^}");     // arbitrary, but unusual
const Location NOWHERE(-1, -1);

template <typename T>
std::string toString(T t) {
    return boost::lexical_cast<std::string>(t);
}

static bool
matchAnyString(const std::vector<std::string> &strings, const std::string &toMatch) {
    BOOST_FOREACH (const std::string &string, strings) {
        if (0==string.compare(toMatch))
            return true;
    }
    return false;
}

SAWYER_EXPORT std::ostream&
operator<<(std::ostream &o, const Location &x) {
    if (x == NOWHERE) {
        o <<"nowhere";
    } else {
        o <<x.idx <<"." <<x.offset;
    }
    return o;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Cursor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT const std::string&
Cursor::arg(const Location &location) const {
    ASSERT_forbid2(atEnd(location), "cursor cannot be positioned at the end of the input");
    return strings_[location.idx];
}

SAWYER_EXPORT std::string
Cursor::rest(const Location &location) const {
    return (location.idx < strings_.size() && location.offset < strings_[location.idx].size() ?
            strings_[location.idx].substr(location.offset) :
            std::string());
}

SAWYER_EXPORT std::string
Cursor::substr(const Location &limit1, const Location &limit2, const std::string &separator) const {
    std::string retval;
    Location begin = limit1, end = limit2;
    if (end < begin)
        std::swap(begin, end);
    while (begin < end) {
        if (begin.idx < end.idx) {
            retval += rest(begin);
            if (end.offset)
                retval += separator;
            ++begin.idx;
            begin.offset = 0;
        } else {
            retval += rest(begin).substr(0, end.offset-begin.offset);
            break;
        }
    }
    return retval;
}

SAWYER_EXPORT Cursor&
Cursor::location(const Location &loc) {
    loc_ = loc;
    if (loc_.idx >= strings_.size()) {
        loc_.idx = strings_.size();
        loc_.offset = 0;
    } else if (loc_.offset > strings_[loc_.idx].size()) {
        loc_.offset = strings_[loc_.idx].size();
    }
    return *this;
}

SAWYER_EXPORT void
Cursor::consumeChars(size_t nchars) {
    if (nchars > 0) {
        ASSERT_forbid2(atEnd(), "cursor cannot be positioned at the end of the input");
        ASSERT_require2(loc_.offset + nchars <= strings_[loc_.idx].size(),
                        "string " + toString(loc_.idx) + " does not have " + toString(nchars) +
                        " character" + (1==nchars?"":"s") + " remaining");
        loc_.offset += nchars;
    }
}

SAWYER_EXPORT void
Cursor::replace(const std::vector<std::string> &args) {
    ASSERT_forbid2(atEnd(), "cursor cannot be positioned at the end of the input");
    std::vector<std::string>::iterator at = strings_.begin() + loc_.idx;
    at = strings_.erase(at);
    strings_.insert(at, args.begin(), args.end());
    Location newloc = loc_;
    newloc.offset = 0;
    location(newloc);
}

SAWYER_EXPORT size_t
Cursor::linearDistance() const {
    size_t retval = 0;
    for (size_t i=0; i<loc_.idx; ++i)
        retval += strings_[i].size();
    retval += loc_.offset;
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT ParsedValue
ValueParser::matchString(const std::string &str) {
    Cursor cursor = str;
    ParsedValue retval = match(cursor);
    if (cursor.atArgBegin())
        throw std::runtime_error("not matched");
    if (!cursor.atArgEnd())
        throw std::runtime_error("extra text after end of value");
    return retval;
}

SAWYER_EXPORT ParsedValue
ValueParser::match(Cursor &cursor) {
    return (*this)(cursor);
}

// Only called by match().  If the subclass doesn't override this, then we try calling the C-string version instead.
SAWYER_EXPORT ParsedValue
ValueParser::operator()(Cursor &cursor) {
    std::string str = cursor.rest();
    const char *s = str.c_str();
    const char *rest = s;
    try {
        ParsedValue retval = (*this)(s, &rest, cursor.location());
        if (NULL!=rest) {
            ASSERT_require(rest>=s && rest<= s+strlen(s));
            cursor.consumeChars(rest-s);
        }
        return retval;
    } catch (const std::runtime_error&) {
        // We must update the cursor location even if an exception is thrown because it indicates that a value
        // was syntactically correct, but not semantically correct. E.g., a mathematical integer that could not be
        // stored as an unsigned type.
        if (NULL!=rest) {
            ASSERT_require(rest>=s && rest<= s+strlen(s));
            cursor.consumeChars(rest-s);
        }
        throw;
    }
}

// only called by ValueParser::operator()(Cursor&)
SAWYER_EXPORT ParsedValue
ValueParser::operator()(const char *s, const char **rest, const Location &loc) {
    throw std::runtime_error("subclass must implement an operator() with a cursor or C strings");
}

SAWYER_EXPORT AnyParser<std::string>::Ptr
anyParser() {
    return AnyParser<std::string>::instance();
}

SAWYER_EXPORT IntegerParser<int>::Ptr
integerParser() {
    return IntegerParser<int>::instance();
}

SAWYER_EXPORT NonNegativeIntegerParser<unsigned>::Ptr
nonNegativeIntegerParser() {
    return NonNegativeIntegerParser<unsigned>::instance();
}

SAWYER_EXPORT PositiveIntegerParser<unsigned>::Ptr
positiveIntegerParser() {
    return PositiveIntegerParser<unsigned>::instance();
}

SAWYER_EXPORT RealNumberParser<double>::Ptr
realNumberParser() {
    return RealNumberParser<double>::instance();
}

SAWYER_EXPORT BooleanParser<bool>::Ptr
booleanParser() {
    return BooleanParser<bool>::instance();
}

SAWYER_EXPORT ParsedValue
StringSetParser::operator()(Cursor &cursor) {
    Location locStart = cursor.location();
    std::string input = cursor.rest();
    size_t bestMatchIdx = (size_t)(-1), bestMatchLen = 0;
    for (size_t i=0; i<strings_.size(); ++i) {
        if (boost::starts_with(input, strings_[i]) && ((size_t)(-1)==bestMatchIdx || strings_[i].size()>bestMatchLen)) {
            bestMatchIdx = i;
            bestMatchLen = strings_[i].size();
        }
    }
    if ((size_t)(-1)==bestMatchIdx)
        throw std::runtime_error("specific word expected");
    cursor.consumeChars(bestMatchLen);
    return ParsedValue(strings_[bestMatchIdx], locStart, strings_[bestMatchIdx], valueSaver());
}

SAWYER_EXPORT ListParser::Ptr
ListParser::limit(size_t minLength, size_t maxLength) {
    if (minLength > maxLength)
        throw std::runtime_error("minimum ListParser length must be less than or equal to maximum length");
    minLength_ = minLength;
    maxLength_ = maxLength;
    return sharedFromThis().dynamicCast<ListParser>();
}

SAWYER_EXPORT ParsedValue
ListParser::operator()(Cursor &cursor) {
    ASSERT_forbid(elements_.empty());
    Location startLoc = cursor.location();
    ExcursionGuard guard(cursor);                       // parsing the list should be all or nothing
    ValueList values;
    std::string sep = "";

    for (size_t i=0; i<maxLength_; ++i) {
        const ParserSep &ps = elements_[std::min(i, elements_.size()-1)];

        // Advance over the value separator
        if (0!=i) {
            if (cursor.atArgBegin() || cursor.atEnd())
                break;                                  // we've advanced over the entire program argument
            std::string str = cursor.rest();
            const char *s = str.c_str();
            boost::regex re("\\A(" + sep + ")");
            boost::cmatch matched;
            if (!regex_search(s, matched, re))
                break;
            cursor.consumeChars(matched.str().size());
        }
        sep = ps.second;

        // Find the next value separator so we can prevent from parsing through it
        size_t endOfValue = cursor.rest().size();
        {
            boost::regex re(sep);
            boost::cmatch matched;
            std::string str = cursor.rest();
            const char *s = str.c_str();
            if (regex_search(s, matched, re))
                endOfValue = matched.position();
        }

        // Parse the value, stopping before the next separator
        Cursor valueCursor(cursor.rest().substr(0, endOfValue));
        ParsedValue value = ps.first->match(valueCursor);
        value.valueLocation(cursor.location());
        cursor.consumeChars(valueCursor.linearDistance());
        values.push_back(value);
    }

    if (values.size()<minLength_ || values.size()>maxLength_) {
        std::ostringstream ss;
        if (minLength_ == maxLength_) {
            ss <<"list with " <<maxLength_ <<" element" <<(1==maxLength_?"":"s") <<" expected (got " <<values.size() <<")";
            throw std::runtime_error(ss.str());
        } else if (minLength_+1 == maxLength_) {
            ss <<"list with " <<minLength_ <<" or " <<maxLength_ <<" element" <<(1==maxLength_?"":"s") <<" expected"
               <<" (got " <<values.size() <<")";
            throw std::runtime_error(ss.str());
        } else {
            std::ostringstream ss;
            ss <<"list with " <<minLength_ <<" to " <<maxLength_ <<" elements expected (got " <<values.size() <<")";
            throw std::runtime_error(ss.str());
        }
    }

    guard.cancel();
    return ParsedValue(values, startLoc, cursor.substr(startLoc), valueSaver());
}

SAWYER_EXPORT StringSetParser::Ptr
stringSetParser(std::string &storage) {
    return StringSetParser::instance(TypedSaver<std::string>::instance(storage));
}

SAWYER_EXPORT StringSetParser::Ptr
stringSetParser() {
    return StringSetParser::instance();
}

SAWYER_EXPORT ListParser::Ptr
listParser(const ValueParser::Ptr &p, const std::string &sep) {
    return ListParser::instance(p, sep);
}

/*******************************************************************************************************************************
 *                                      Actions
 *******************************************************************************************************************************/

SAWYER_EXPORT void
ShowVersion::operator()(const ParserResult&) {
    std::cerr <<versionString_ <<"\n";
}

SAWYER_EXPORT void
ShowVersionAndExit::operator()(const ParserResult &parserResult) {
    ShowVersion::operator()(parserResult);
    exit(exitStatus_);
}

SAWYER_EXPORT void
ShowHelp::operator()(const ParserResult &parserResult) {
    parserResult.parser().emitDocumentationToPager();
}

SAWYER_EXPORT void
ShowHelpAndExit::operator()(const ParserResult &parserResult) {
    ShowHelp::operator()(parserResult);
    exit(exitStatus_);
}

SAWYER_EXPORT void
ConfigureDiagnostics::operator()(const ParserResult &parserResult) {
    BOOST_FOREACH (const ParsedValue &value, parserResult.parsed(switchKey_)) {
        if (0==value.string().compare("list")) {
            std::cout <<"Logging facilities status\n"
                //       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx (80 cols)
                      <<"  Letters indicate a stream that is enabled; hyphens indicate disabled.\n"
                      <<"  D=debug, T=trace, H=where, I=info, W=warning, E=error, F=fatal\n";
            facilities_.print(std::cout);
            if (exitOnHelp_)
                exit(0);
        } else if (0==value.string().compare("help")) {
            //           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx (80 cols)
            std::cout <<"Logging is controlled with a simple language consisting of a comma-separated\n"
                      <<"list of facility specifications, where each facility specification is a\n"
                      <<"facility name (use \"list\" to get a listing of facilities) followed by a\n"
                      <<"parentheses-enclosed, comma-separated list of importance specifications. An\n"
                      <<"importance specification is an importance name (debug, trace, where, info,\n"
                      <<"warn, error, fatal). Each importance name may be preceded by a bang (\"!\")\n"
                      <<"to disable that stream, or a relational operator (\"<\", \"<=\", \">\", or \">=\")\n"
                      <<"to enable related streams.  The special name \"all\" means all importance levels\n"
                      <<"and the name \"none\" is an alias for \"!all\" (i.e., disable all levels).  If\n"
                      <<"the facility name and parentheses are omitted, then the naked importance levels\n"
                      <<"affect all facilities.  The specification is processed from left to right.\n"
                      <<"Examples:\n"
                      <<"   help                           -- show this documentation\n"
                      <<"   list                           -- list status for all streams\n"
                      <<"   all                            -- turn on everything\n"
                      <<"   all,!debug                     -- turn on everything except debug\n"
                      <<"   none, foo(debug)               -- turn off everything but foo's debug\n"
                      <<"   none, >=info, foo(none,debug)  -- info and greater everywhere; foo's debug\n";
            if (exitOnHelp_)
                exit(0);
        } else {
            std::string errorMessage = facilities_.control(value.string());
            if (!errorMessage.empty())
                throw std::runtime_error(errorMessage);
        }
    }
}

SAWYER_EXPORT ShowVersion::Ptr
showVersion(const std::string &versionString) { return ShowVersion::instance(versionString); }

SAWYER_EXPORT ShowVersionAndExit::Ptr
showVersionAndExit(const std::string &versionString, int exitStatus) {
    return ShowVersionAndExit::instance(versionString, exitStatus);
}

SAWYER_EXPORT ShowHelp::Ptr
showHelp() { return ShowHelp::instance(); }

SAWYER_EXPORT ShowHelpAndExit::Ptr
showHelpAndExit(int exitStatus) { return ShowHelpAndExit::instance(exitStatus); }

SAWYER_EXPORT ConfigureDiagnostics::Ptr
configureDiagnostics(const std::string &switchKey, Message::Facilities &facilities, bool exitOnHelp) {
    return ConfigureDiagnostics::instance(switchKey, facilities, exitOnHelp);
}

/*******************************************************************************************************************************
 *                                      Parsed values
 *******************************************************************************************************************************/

// A variety of common integer types
template<typename T>
static T fromSigned(const boost::any &v) {
    if (v.type() == typeid(boost::int64_t)) {
        return boost::numeric_cast<T>(boost::any_cast<boost::int64_t>(v));
    } else if (v.type() == typeid(long long)) {
        return boost::numeric_cast<T>(boost::any_cast<long long>(v));
    } else if (v.type() == typeid(long)) {
        return boost::numeric_cast<T>(boost::any_cast<long>(v));
    } else if (v.type() == typeid(int)) {
        return boost::numeric_cast<T>(boost::any_cast<int>(v));
    } else if (v.type() == typeid(short)) {
        return boost::numeric_cast<T>(boost::any_cast<short>(v));
    } else if (v.type() == typeid(signed char)) {
        return boost::numeric_cast<T>(boost::any_cast<signed char>(v));
    } else {
        return boost::any_cast<T>(v);
    }
}

template<typename T>
static T fromUnsigned(const boost::any &v) {
    if (v.type() == typeid(boost::uint64_t)) {
        return boost::numeric_cast<T>(boost::any_cast<boost::uint64_t>(v));
    } else if (v.type() == typeid(unsigned long long)) {
        return boost::numeric_cast<T>(boost::any_cast<unsigned long long>(v));
    } else if (v.type() == typeid(unsigned long)) {
        return boost::numeric_cast<T>(boost::any_cast<unsigned long>(v));
    } else if (v.type() == typeid(unsigned int)) {
        return boost::numeric_cast<T>(boost::any_cast<unsigned int>(v));
    } else if (v.type() == typeid(unsigned short)) {
        return boost::numeric_cast<T>(boost::any_cast<unsigned short>(v));
    } else if (v.type() == typeid(unsigned char)) {
        return boost::numeric_cast<T>(boost::any_cast<unsigned char>(v));
    } else if (v.type() == typeid(size_t)) {
        return boost::numeric_cast<T>(boost::any_cast<size_t>(v));
    } else {
        return boost::any_cast<T>(v);
    }
}

template<typename T>
static T fromInteger(const boost::any &v) {
    try {
        return fromSigned<T>(v);
    } catch (const boost::bad_any_cast&) {
    }
    try {
        return fromUnsigned<T>(v);
    } catch (const boost::bad_any_cast&) {
    }
    if (v.type() == typeid(bool)) {
        return boost::any_cast<bool>(v);
    } else {
        return boost::any_cast<T>(v);                   // try blind luck
    }
}

// A variety of common floating point types
template<typename T>
T fromFloatingPoint(const boost::any &v) {
    if (v.type() == typeid(double)) {
        return boost::numeric::converter<T, double>::convert(boost::any_cast<double>(v));
    } else if (v.type() == typeid(float)) {
        return boost::numeric::converter<T, float>::convert(boost::any_cast<float>(v));
    } else {
        return fromInteger<T>(v);
    }
}

SAWYER_EXPORT int
ParsedValue::asInt() const {
    return fromInteger<int>(value_);
}

SAWYER_EXPORT unsigned
ParsedValue::asUnsigned() const {
    return fromInteger<unsigned>(value_);
}

SAWYER_EXPORT long
ParsedValue::asLong() const {
    return fromInteger<long>(value_);
}

SAWYER_EXPORT unsigned long
ParsedValue::asUnsignedLong() const {
    return fromInteger<unsigned long>(value_);
}

SAWYER_EXPORT boost::int64_t
ParsedValue::asInt64() const {
    return fromInteger<boost::int64_t>(value_);
}

SAWYER_EXPORT boost::uint64_t
ParsedValue::asUnsigned64() const {
    return fromInteger<boost::int64_t>(value_);
}

SAWYER_EXPORT double
ParsedValue::asDouble() const {
    return fromFloatingPoint<double>(value_);
}

SAWYER_EXPORT float
ParsedValue::asFloat() const {
    return fromFloatingPoint<float>(value_);
}

SAWYER_EXPORT bool
ParsedValue::asBool() const {
    return fromInteger<boost::uint64_t>(value_) != 0;
}

SAWYER_EXPORT std::string
ParsedValue::asString() const {
    try {
        boost::int64_t x = fromSigned<boost::int64_t>(value_);
        return toString(x);
    } catch (const boost::bad_any_cast&) {
    }
    try {
        boost::uint64_t x = fromUnsigned<boost::int64_t>(value_);
        return toString(x);
    } catch (const boost::bad_any_cast&) {
    }
    try {
        double x = fromFloatingPoint<double>(value_);
        return toString(x);
    } catch (const boost::bad_any_cast&) {
    }
    try {
        bool x = boost::any_cast<bool>(value_);
        return toString(x);
    } catch (const boost::bad_any_cast&) {
    }
    std::string x = boost::any_cast<std::string>(value_);
    return x;
}

SAWYER_EXPORT void
ParsedValue::save() const {
    if (valueSaver_)
        valueSaver_->save(value_);

    if (value_.type() == typeid(ListParser::ValueList)) {
        const ListParser::ValueList &values = boost::any_cast<ListParser::ValueList>(value_);
        BOOST_FOREACH (const ParsedValue &pval, values)
            pval.save();
    }
}

SAWYER_EXPORT void
ParsedValue::print(std::ostream &o) const {
    o <<"{switch=\"" <<switchString_ <<"\" at " <<switchLocation_ <<" key=\"" <<switchKey_ <<"\""
      <<"; value str=\"" <<valueString_ <<"\" at " <<valueLocation_
      <<"; seq={s" <<switchSequence_ <<", k" <<keySequence_ <<"}"
      <<"}";
}

SAWYER_EXPORT std::ostream&
operator<<(std::ostream &o, const ParsedValue &x) {
    x.print(o);
    return o;
}

/*******************************************************************************************************************************
 *                                      Switch arguments declarations
 *******************************************************************************************************************************/

SAWYER_EXPORT std::string
SwitchArgument::nameAsText() const {
    std::ostringstream ss;
    Markup::Parser().parse(name_).emit(ss, Markup::TextFormatter::instance());
    return ss.str();
}

/*******************************************************************************************************************************
 *                                      Switch Descriptors
 *******************************************************************************************************************************/

SAWYER_EXPORT ParsingProperties
ParsingProperties::inherit(const ParsingProperties &base) const {
    ParsingProperties retval;
    if (inheritLongPrefixes)
        retval.longPrefixes = base.longPrefixes;
    retval.longPrefixes.insert(retval.longPrefixes.end(), longPrefixes.begin(), longPrefixes.end());
    if (inheritShortPrefixes)
        retval.shortPrefixes = base.shortPrefixes;
    retval.shortPrefixes.insert(retval.shortPrefixes.end(), shortPrefixes.begin(), shortPrefixes.end());
    if (inheritValueSeparators)
        retval.valueSeparators = base.valueSeparators;
    retval.valueSeparators.insert(retval.valueSeparators.end(), valueSeparators.begin(), valueSeparators.end());
    return retval;
}

SAWYER_EXPORT void
Switch::init(const std::string &longName, char shortName) {
    if (shortName)
        shortNames_ = std::string(1, shortName);
    if (!longName.empty()) {
        longNames_.push_back(longName);
        key_ = documentationKey_ = longName;
    } else if (shortName) {
        key_ = documentationKey_ = std::string(1, shortName);
    } else {
        throw std::runtime_error("every Switch must have either a long or short name");
    }
}

SAWYER_EXPORT Switch&
Switch::longName(const std::string &name) {
    if (name.empty())
        throw std::runtime_error("switch long name cannot be empty");
    longNames_.push_back(name);
    return *this;
}

SAWYER_EXPORT std::string
Switch::synopsisForArgument(const SwitchArgument &sa) const {
    std::string retval;
    if (sa.isOptional())
        retval += "[";

    // If the name is a single word that is all lower-case (digits, hyphens, and underscores are also allowed except as the
    // leading character) then format it as a variable.
    std::string s = sa.name();
    if (boost::find_regex(s, boost::regex("^[a-z][-_a-z0-9]*$"))) {
        retval += "@v{" + s + "}";
    } else {
        retval += s;
    }

    if (sa.isOptional())
        retval += "]";
    return retval;
}

SAWYER_EXPORT std::string
Switch::synopsis() const {
    if (!synopsis_.empty())
        return synopsis_;

    std::vector<std::string> perName;
    BOOST_FOREACH (const std::string &name, longNames_) {
        std::string s = "@s{" + name +"}";
        BOOST_FOREACH (const SwitchArgument &sa, arguments_)
            s += " " + synopsisForArgument(sa);
        perName.push_back(s);
    }
    BOOST_FOREACH (char name, shortNames_) {
        std::string s = "@s{" + std::string(1, name) + "}";
        BOOST_FOREACH (const SwitchArgument &sa, arguments_)
            s += " " + synopsisForArgument(sa);
        perName.push_back(s);
    }
    return boost::join(perName, "; ");
}

SAWYER_EXPORT Switch&
Switch::resetLongPrefixes(const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4) {
    properties_.inheritLongPrefixes = false;
    properties_.longPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT Switch&
Switch::resetShortPrefixes(const std::string &s1, const std::string &s2, const std::string &s3,
                           const std::string &s4) {
    properties_.inheritShortPrefixes = false;
    properties_.shortPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT Switch&
Switch::resetValueSeparators(const std::string &s1, const std::string &s2, const std::string &s3,
                             const std::string &s4) {
    properties_.inheritValueSeparators = false;
    properties_.valueSeparators.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s4);
    return *this;
}

SAWYER_EXPORT Switch&
Switch::argument(const std::string &name, const ValueParser::Ptr &parser) {
    return argument(SwitchArgument(name, parser));
}

SAWYER_EXPORT Switch&
Switch::argument(const std::string &name, const ValueParser::Ptr &parser, const std::string &defaultValueStr) {
    return argument(SwitchArgument(name, parser, defaultValueStr));
}

SAWYER_EXPORT size_t
Switch::nRequiredArguments() const {
    size_t retval = 0;
    BOOST_FOREACH (const SwitchArgument &sa, arguments_) {
        if (sa.isRequired())
            ++retval;
    }
    return retval;
}

SAWYER_EXPORT std::runtime_error
Switch::notEnoughArguments(const std::string &switchString, const Cursor &cursor, size_t nargs) const {
    std::ostringstream ss;
    ss <<"not enough arguments for " <<switchString <<" (found " <<nargs <<" but expected ";
    if (arguments_.size() != nRequiredArguments())
        ss <<"at least ";
    ss <<nRequiredArguments() <<")";
    return std::runtime_error(ss.str());
}

SAWYER_EXPORT std::runtime_error
Switch::noSeparator(const std::string &switchString, const Cursor &cursor,
                    const ParsingProperties &props) const {
    std::string s;
    bool hasSpaceSeparator = matchAnyString(props.valueSeparators, " ");
    if ((cursor.atArgBegin() || cursor.atEnd()) && hasSpaceSeparator) {
        s = "required argument for " + switchString + " is missing";
    } else {
        s = "expected one of the following separators between " + switchString + " and its argument:";
        BOOST_FOREACH (std::string sep, props.valueSeparators) {
            if (0!=sep.compare(" "))
                s += " \"" + sep + "\"";
        }
    }
    return std::runtime_error(s);
}

SAWYER_EXPORT std::runtime_error
Switch::extraTextAfterSwitch(const std::string &switchString, const Location &endOfSwitch,
                             const Cursor &cursor, const ParsingProperties &props,
                             const ParsedValues &values) const {

    std::string extraText = cursor.rest();
    if (!values.empty()) {
        if (extraText.empty()) {
            return std::runtime_error("unexpected empty-string argument after " + switchString +
                                      " default argument \"" + values.back().string() + "\"");
        } else {
            return std::runtime_error("extra text after " + switchString + " default argument \"" +
                                      values.back().string() + "\"; extra text is \"" + extraText + "\"");
        }
    }
    

    BOOST_FOREACH (std::string sep, props.valueSeparators) {
        if (0!=sep.compare(" ")) {
            if (boost::starts_with(cursor.rest(endOfSwitch), sep))
                return std::runtime_error("unexpected argument for " + switchString);
        }
    }

    return std::runtime_error("unrecognized switch " + switchString + cursor.substr(endOfSwitch) + extraText);
}

SAWYER_EXPORT std::runtime_error
Switch::extraTextAfterArgument(const Cursor &cursor, const ParsedValue &value) const {
    return std::runtime_error("value \"" + value.string() + "\" unexpectedly followed by \"" + cursor.rest() + "\"");
}
    
SAWYER_EXPORT std::runtime_error
Switch::missingArgument(const std::string &switchString, const Cursor &cursor,
                        const SwitchArgument &sa, const std::string &reason) const {
    std::string str = "required argument for " + switchString + " is missing; for " + sa.nameAsText();
    if (!reason.empty())
        str += ": " + reason;
    return std::runtime_error(str);
}

SAWYER_EXPORT std::runtime_error
Switch::malformedArgument(const std::string &switchString, const Cursor &cursor,
                          const SwitchArgument &sa, const std::string &reason) const {
    std::string str = "argument for " + switchString + " is invalid; for " + sa.nameAsText();
    if (!reason.empty())
        str += ": " + reason;
    return std::runtime_error(str);
}

SAWYER_EXPORT size_t
Switch::matchLongName(Cursor &cursor, const ParsingProperties &props, const std::string &name) const {
    ASSERT_require(cursor.atArgBegin());
    BOOST_FOREACH (const std::string &prefix, props.longPrefixes) {
        if (boost::starts_with(cursor.arg(), prefix)) {
            std::string rest = cursor.arg().substr(prefix.size());
            if (boost::starts_with(rest, name)) {
                size_t retval = prefix.size() + name.size();
                rest = rest.substr(name.size());
                if (rest.empty()) {
                    cursor.consumeChars(retval);
                    return retval;                  // switch name matches to end of program argument
                }
                if (0==arguments_.size()) {
                    cursor.consumeChars(retval);
                    return retval;
                }
                BOOST_FOREACH (const std::string &sep, props.valueSeparators) {
                    if (0!=sep.compare(" ") && boost::starts_with(rest, sep)) {
                        cursor.consumeChars(retval);
                        return retval;              // found prefix, name, and separator for switch with args
                    }
                }
            }
        }
    }
    return 0;
}

SAWYER_EXPORT size_t
Switch::matchShortName(Cursor &cursor, const ParsingProperties &props, std::string &name /*out*/) const {
    BOOST_FOREACH (const std::string &prefix, props.shortPrefixes) {
        if (boost::starts_with(cursor.arg(), prefix)) {
            if (prefix.size() >= cursor.location().offset && prefix.size() < cursor.arg().size()) {
                // name must immediately follow the prefix
                if (strchr(shortNames_.c_str(), cursor.arg()[prefix.size()])) {
                    size_t retval = prefix.size() + 1;
                    name = cursor.arg().substr(0, retval);
                    Location p = cursor.location();
                    p.offset = retval;
                    cursor.location(p);
                    return retval;
                }
            } else if (prefix.size() < cursor.arg().size()) {
                if (strchr(shortNames_.c_str(), cursor.rest()[0])) {
                    name = cursor.arg().substr(0, prefix.size()) + cursor.rest().substr(0, 1);
                    cursor.consumeChars(1);
                    return prefix.size() + 1;
                }
            }
        }
    }
    name = "";
    return 0;
}

// optionally explodes a vector value into separate values
SAWYER_EXPORT bool
Switch::explode(ParsedValues &pvals /*in,out*/) const {
    if (!explosiveLists_)
        return false;

    bool retval = false;
    ParsedValues pvals2;
    BOOST_FOREACH (const ParsedValue &pval1, pvals) {
        if (pval1.value().type()==typeid(ListParser::ValueList)) {
            ListParser::ValueList elmts = boost::any_cast<ListParser::ValueList>(pval1.value());
            BOOST_FOREACH (const ParsedValue &elmt, elmts) {
                pvals2.push_back(elmt);
                retval = true;
            }
        } else {
            pvals2.push_back(pval1);
        }
    }
    pvals = pvals2;
    return retval;
}

// cursor is initially at the first character of the first switch argument
SAWYER_EXPORT size_t
Switch::matchArguments(const std::string &switchString, const Location &endOfSwitch, Cursor &cursor /*in,out*/,
                       const ParsingProperties &props, ParsedValues &result /*out*/, bool finalAlignment) const {
    ASSERT_forbid(arguments_.empty());

    ParsedValues parsedValues;
    ExcursionGuard guard(cursor);
    size_t nValuesParsed = 0;
    size_t switchIdx = cursor.location().idx;           // which program argument holds the switch name
    if (switchIdx>0 && (cursor.atArgBegin() || cursor.atEnd()))
        --switchIdx;

    // Parse arguments, or use defaults if allowed.
    const SwitchArgument *lastParsedArgument = NULL;
    ParsedValue lastParsedValue;
    for (size_t argno=0; argno<arguments_.size(); ++argno) {
        const SwitchArgument &sa = arguments_[argno];
        Location valueLocation = cursor.location();
        try {
            ParsedValue value = sa.parser()->match(cursor);
            parsedValues.push_back(value);
            if (cursor.atArgEnd()) {
                cursor.consumeArg();
            } else if (finalAlignment || argno>0) {
                throw extraTextAfterArgument(cursor, value);
            }
            ++nValuesParsed;
            lastParsedArgument = &sa;
            lastParsedValue = value;
        } catch (const std::runtime_error &e) {
            if (sa.isRequired()) {
                throw cursor.location()==valueLocation ?
                    missingArgument(switchString, cursor, sa, e.what()) :
                    malformedArgument(switchString, cursor, sa, e.what());
            }
            cursor.location(valueLocation);
            parsedValues.push_back(sa.defaultValue());
        }
    }

    // Regardless of whether we parsed arguments, we need to check final alignment.  However, if the last value was a parsed
    // value from the loop above (as opposed to a default value) then we've already checked this and advanced to the next
    // program argument.
    bool lastArgWasParsed = !parsedValues.empty() && NOWHERE!=parsedValues.back().valueLocation();
    if (finalAlignment && !lastArgWasParsed && !cursor.atArgBegin() && !cursor.atEnd()) {
        if (lastParsedArgument) {
            throw extraTextAfterArgument(cursor, lastParsedValue);
        } else {
            throw extraTextAfterSwitch(switchString, endOfSwitch, cursor, props, parsedValues);
        }
    }

    explode(result);
    guard.cancel();
    result.insert(result.end(), parsedValues.begin(), parsedValues.end());
    return nValuesParsed;
}

// cursor is initially immediately after the switch name
SAWYER_EXPORT void
Switch::matchLongArguments(const std::string &switchString, Cursor &cursor /*in,out*/, const ParsingProperties &props,
                           ParsedValues &result /*out*/) const {
    ExcursionGuard guard(cursor);

    // If the switch has no declared arguments use its intrinsic value.
    if (arguments_.empty()) {
        if (!cursor.atArgEnd())
            throw extraTextAfterSwitch(switchString, cursor.location(), cursor, props, result);
        result.push_back(intrinsicValue_);
        cursor.consumeArg();
        guard.cancel();
        return;
    }

    // Try to match the name/value separator.  Advance the cursor to the first character of the first value.
    Location endOfSwitch = cursor.location();
    bool didMatchSeparator = false;
    if (cursor.atArgEnd()) {
        if (matchAnyString(props.valueSeparators, " ")) {
            didMatchSeparator = true;
            cursor.consumeArg();
        }
    } else {
        std::string s = cursor.rest();
        BOOST_FOREACH (const std::string &sep, props.valueSeparators) {
            if (boost::starts_with(s, sep)) {
                cursor.consumeChars(sep.size());
                didMatchSeparator = true;
                break;
            }
        }
    }
    if ((!didMatchSeparator || cursor.atEnd()) && nRequiredArguments()>0)
        throw noSeparator(switchString, cursor, props);

    // Parse the arguments for this switch now that we've consumed the prefix, switch name, and argument separators.
    matchArguments(switchString, endOfSwitch, cursor, props, result /*out*/, true /*finalAlignment*/);
    guard.cancel();
}

// cursor is initially immediately after the switch name
SAWYER_EXPORT void
Switch::matchShortArguments(const std::string &switchString, Cursor &cursor /*in,out*/, const ParsingProperties &props,
                            ParsedValues &result /*out*/, bool mayNestle) const {
    ExcursionGuard guard(cursor);

    // If the switch has no declared arguments, then parse its default.
    if (arguments_.empty()) {
        if (cursor.atArgEnd()) {
            cursor.consumeArg();
        } else if (!mayNestle) {
            throw extraTextAfterSwitch(switchString, guard.startingLocation(), cursor, props, result);
        }
        result.push_back(intrinsicValue_);
        guard.cancel();
        return;
    }

    // The switch argument may start immediately after the switch name, or in the next program argument.
    bool finalAlignment = !mayNestle;
    if (cursor.atArgEnd()) {
        cursor.consumeArg();
        finalAlignment = true;
    }

    // Parse the arguments for this switch.
    matchArguments(switchString, guard.startingLocation(), cursor, props, result /*out*/, finalAlignment);
    guard.cancel();
}

/*******************************************************************************************************************************
 *                                      SwitchGroup
 *******************************************************************************************************************************/

SAWYER_EXPORT SwitchGroup&
SwitchGroup::resetLongPrefixes(const std::string &s1, const std::string &s2,
                               const std::string &s3, const std::string &s4) {
    properties_.inheritLongPrefixes = false;
    properties_.longPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT SwitchGroup&
SwitchGroup::resetShortPrefixes(const std::string &s1, const std::string &s2,
                                const std::string &s3, const std::string &s4) {
    properties_.inheritShortPrefixes = true;
    properties_.shortPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT SwitchGroup&
SwitchGroup::resetValueSeparators(const std::string &s1, const std::string &s2,
                                  const std::string &s3, const std::string &s4) {
    properties_.inheritValueSeparators = false;
    properties_.valueSeparators.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s4);
    return *this;
}

SAWYER_EXPORT SwitchGroup&
SwitchGroup::insert(const Switch &sw) {
    switches_.push_back(sw);
    return *this;
}

SAWYER_EXPORT SwitchGroup&
SwitchGroup::insert(const SwitchGroup &other) {
    BOOST_FOREACH (const Switch &sw, other.switches_)
        switches_.push_back(sw);
    return *this;
}

SAWYER_EXPORT bool
SwitchGroup::nameExists(const std::string &s) {
    for (size_t i=0; i<switches_.size(); ++i) {
        const std::vector<std::string> &names = switches_[i].longNames();
        if (std::find(names.begin(), names.end(), s)!=names.end() ||
            (1==s.size() && boost::contains(switches_[i].shortNames(), s)))
            return true;
    }
    return false;
}

SAWYER_EXPORT const Switch&
SwitchGroup::getByName(const std::string &s) {
    for (size_t i=0; i<switches_.size(); ++i) {
        const std::vector<std::string> &names = switches_[i].longNames();
        if (std::find(names.begin(), names.end(), s)!=names.end() ||
            (1==s.size() && boost::contains(switches_[i].shortNames(), s)))
            return switches_[i];
    }
    throw std::runtime_error("switch \"" + s + "\" not found\n");
}

SAWYER_EXPORT bool
SwitchGroup::keyExists(const std::string &s) {
    for (size_t i=0; i<switches_.size(); ++i) {
        if (0==switches_[i].key().compare(s))
            return true;
    }
    return false;
}

SAWYER_EXPORT const Switch&
SwitchGroup::getByKey(const std::string &s) {
    for (size_t i=0; i<switches_.size(); ++i) {
        if (0==switches_[i].key().compare(s))
            return switches_[i];
    }
    throw std::runtime_error("switch key \"" + s + "\" not found\n");
}

/*******************************************************************************************************************************
 *                                      Parser results
 *******************************************************************************************************************************/

// Do not save the 'sw' pointer because we have no control over when the user will destroy the object.
// This should be called for at most one switch occurrence at a time.
SAWYER_EXPORT void
ParserResult::insertValuesForSwitch(const ParsedValues &pvals, const Parser *parser, const Switch *sw) {
    ASSERT_not_null(sw);
    std::string key = sw->key();
    std::string name = sw->preferredName();

    // How to save this value
    bool shouldSave = true;
    
    switch (sw->whichValue()) {
        case SAVE_NONE:
            if (!pvals.empty())
                throw std::runtime_error(pvals.front().switchString() + " is illegal here");
        case SAVE_ONE:
            if (!keyIndex_.getOrDefault(key).empty() && !pvals.empty())
                throw std::runtime_error("switch key \"" + key + "\" cannot appear multiple times (" +
                                         pvals.front().switchString() + ")");
            break;
        case SAVE_FIRST:
            if (!keyIndex_.getOrDefault(key).empty())
                shouldSave = false;                     // skip this value since we already saved one
            break;
        case SAVE_LAST:
            keyIndex_.insertDefault(key);
            break;
        case SAVE_ALL:
            break;
        case SAVE_AUGMENTED:
            ValueAugmenter::Ptr f = sw->valueAugmenter();
            if (f!=NULL && !keyIndex_.getOrDefault(key).empty()) {
                ParsedValues oldValues;
                BOOST_FOREACH (size_t idx, keyIndex_[key])
                    oldValues.push_back(values_[idx]);
                ParsedValues newValues = (*f)(oldValues, pvals);
                keyIndex_[key].clear();
                BOOST_FOREACH (const ParsedValue &pval, newValues)
                    insertOneValue(pval, sw);
                return;
            }
            keyIndex_.insertDefault(key);               // act like SAVE_LAST
            break;
    }

    BOOST_FOREACH (const ParsedValue &pval, pvals)
        insertOneValue(pval, sw, shouldSave);
}

SAWYER_EXPORT void
ParserResult::insertOneValue(const ParsedValue &pval, const Switch *sw, bool saveValue) {
    // Get sequences for this value and update the value.
    const std::string &key = sw->key();
    const std::string &name = sw->preferredName();
    size_t keySequence = keyIndex_.getOrDefault(key).size();
    size_t switchSequence = switchIndex_.getOrDefault(name).size();
    size_t idx = values_.size();
    values_.push_back(pval);
    values_.back().switchKey(key);
    values_.back().sequenceInfo(keySequence, switchSequence);
    argvIndex_.insertMaybeDefault(pval.switchLocation()).push_back(idx);

    // Associate the value with a key and switch name
    if (saveValue) {
        keyIndex_.insertMaybeDefault(key).push_back(idx);
        switchIndex_.insertMaybeDefault(name).push_back(idx);
        actions_.insert(key, sw->action());

#if 0 /*DEBUGGING [Robb Matzke 2014-02-18]*/
        std::cerr <<"    " <<values_.back() <<"\n";
#endif
    }
}
    
SAWYER_EXPORT void
ParserResult::skip(const Location &loc) {
    skippedIndex_.push_back(loc.idx);
}

SAWYER_EXPORT void
ParserResult::terminator(const Location &loc) {
    terminators_.push_back(loc.idx);
}

SAWYER_EXPORT const ParserResult&
ParserResult::apply() const {
    // Save values into variables
    BOOST_FOREACH (const std::vector<size_t> &indexes, keyIndex_.values()) {
        BOOST_FOREACH (size_t idx, indexes) {
            values_[idx].save();
        }
    }
    // Run actions
    BOOST_FOREACH (const SwitchAction::Ptr &action, actions_.values()) {
        if (action)
            action->run(*this);
    }

    return *this;
}

SAWYER_EXPORT const ParsedValue&
ParserResult::parsed(const std::string &switchKey, size_t idx) const {
    return values_[keyIndex_[switchKey][idx]];
}

SAWYER_EXPORT ParsedValues
ParserResult::parsed(const std::string &switchKey) const {
    ParsedValues retval;
    BOOST_FOREACH (size_t idx, keyIndex_[switchKey])
        retval.push_back(values_[idx]);
    return retval;
}

SAWYER_EXPORT std::vector<std::string>
ParserResult::skippedArgs() const {
    std::vector<std::string> retval;
    BOOST_FOREACH (size_t idx, skippedIndex_)
        retval.push_back(cursor_.strings()[idx]);
    return retval;
}

SAWYER_EXPORT std::vector<std::string>
ParserResult::unreachedArgs() const {
    std::vector<std::string> retval;
    for (size_t i=cursor_.location().idx; i<cursor_.strings().size(); ++i)
        retval.push_back(cursor_.strings()[i]);
    return retval;
}

SAWYER_EXPORT std::vector<std::string>
ParserResult::unparsedArgs(bool includeTerminators) const {
    std::set<size_t> indexes;
    BOOST_FOREACH (size_t idx, skippedIndex_)
        indexes.insert(idx);
    if (includeTerminators) {
        BOOST_FOREACH (size_t idx, terminators_)
            indexes.insert(idx);
    }
    for (size_t i=cursor_.location().idx; i<cursor_.strings().size(); ++i)
        indexes.insert(i);

    std::vector<std::string> retval;
    BOOST_FOREACH (size_t idx, indexes)
        retval.push_back(cursor_.strings()[idx]);
    return retval;
}

SAWYER_EXPORT std::vector<std::string>
ParserResult::parsedArgs() const {
    std::set<size_t> indexes;

    // Program arguments that have parsed switches, and the locations of the switch values
    BOOST_FOREACH (const ArgvIndex::Node &node, argvIndex_.nodes()) {
        indexes.insert(node.key().idx);
        BOOST_FOREACH (size_t valueIdx, node.value()) {
            const Location valueLocation = values_[valueIdx].valueLocation();
            if (valueLocation != NOWHERE)
                indexes.insert(valueLocation.idx);
        }
    }

    BOOST_FOREACH (size_t idx, terminators_)
        indexes.insert(idx);

    std::vector<std::string> retval;
    BOOST_FOREACH (size_t idx, indexes)
        retval.push_back(cursor_.strings()[idx]);
    return retval;
}


/*******************************************************************************************************************************
 *                                      Parser
 *******************************************************************************************************************************/

SAWYER_EXPORT void
Parser::init() {
    properties_.longPrefixes.push_back("--");           // as in "--version"
    properties_.shortPrefixes.push_back("-");           // as in "-V"
    properties_.valueSeparators.push_back("=");         // as in "--switch=value"
    properties_.valueSeparators.push_back(" ");         // switch value is in next program argument
    terminationSwitches_.push_back("--");
    inclusionPrefixes_.push_back("@");
}

SAWYER_EXPORT Parser&
Parser::resetLongPrefixes(const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4) {
    properties_.inheritLongPrefixes = false;
    properties_.longPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.longPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT Parser&
Parser::resetShortPrefixes(const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4) {
    properties_.inheritShortPrefixes = false;
    properties_.shortPrefixes.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.shortPrefixes.push_back(s4);
    return *this;
}

SAWYER_EXPORT Parser&
Parser::resetValueSeparators(const std::string &s1, const std::string &s2,
                             const std::string &s3, const std::string &s4) {
    properties_.inheritValueSeparators = false;
    properties_.valueSeparators.clear();
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        properties_.valueSeparators.push_back(s4);
    return *this;
}

SAWYER_EXPORT Parser&
Parser::resetTerminationSwitches(const std::string &s1, const std::string &s2,
                                 const std::string &s3, const std::string &s4) {
    terminationSwitches_.clear();
    if (0!=s1.compare(STR_NONE))
        terminationSwitches_.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        terminationSwitches_.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        terminationSwitches_.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        terminationSwitches_.push_back(s4);
    return *this;
}

SAWYER_EXPORT Parser&
Parser::resetInclusionPrefixes(const std::string &s1, const std::string &s2,
                               const std::string &s3, const std::string &s4) {
    inclusionPrefixes_.clear();
    if (0!=s1.compare(STR_NONE))
        inclusionPrefixes_.push_back(s1);
    if (0!=s1.compare(STR_NONE))
        inclusionPrefixes_.push_back(s2);
    if (0!=s1.compare(STR_NONE))
        inclusionPrefixes_.push_back(s3);
    if (0!=s1.compare(STR_NONE))
        inclusionPrefixes_.push_back(s4);
    return *this;
}

SAWYER_EXPORT ParserResult
Parser::parse(int argc, char *argv[]) {
    std::vector<std::string> args(argv+1, argv+argc);
    return parse(args);
}

SAWYER_EXPORT ParserResult
Parser::parse(const std::vector<std::string> &programArguments) {
    if (errorStream_) {
        try {
            return parseInternal(programArguments);
        } catch (const std::runtime_error &e) {
            *errorStream_ << e.what() <<"\n";
            if (exitMessage_) {
                *errorStream_ <<*exitMessage_ <<"\n";
            } else {
                BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
                    ParsingProperties sgProps = sg.properties().inherit(properties_);
                    BOOST_FOREACH (const Switch &sw, sg.switches()) {
                        BOOST_FOREACH (const std::string &name, sw.longNames()) {
                            if (0==name.compare("help")) {
                                ParsingProperties swProps = sw.properties().inherit(sgProps);
                                std::string prefix = swProps.longPrefixes.empty() ? std::string() : swProps.longPrefixes.front();
                                *errorStream_ <<"invoke with '" <<prefix <<"help' for usage information.\n";
                                exit(1);
                            }
                        }
                    }
                }
            }
            exit(1);
        }
    } else {
        return parseInternal(programArguments);
    }
}

SAWYER_EXPORT ParserResult
Parser::parseInternal(const std::vector<std::string> &programArguments) {
    ParserResult result(*this, programArguments);
    Cursor &cursor = result.cursor();

    while (!cursor.atEnd()) {
        ASSERT_require(cursor.atArgBegin());

        // Check for termination switch.
        BOOST_FOREACH (std::string termination, terminationSwitches_) {
            if (0==cursor.arg().compare(termination)) {
                result.terminator(cursor.location());
                cursor.consumeArg();
                return result;
            }
        }

        // Check for file inclusion switch.
        bool inserted = false;
        BOOST_FOREACH (std::string prefix, inclusionPrefixes_) {
            const std::string &arg = cursor.arg();
            if (boost::starts_with(arg, prefix) && arg.size() > prefix.size()) {
                std::string filename = arg.substr(prefix.size());
                std::vector<std::string> args = readArgsFromFile(filename);
                cursor.replace(args);
                inserted = true;
            }
        }
        if (inserted)
            continue;
        
        // Does this look like a switch (even one that we might not know about)?
        bool isSwitch = apparentSwitch(cursor);
        if (!isSwitch) {
            if (skipNonSwitches_) {
                result.skip(cursor.location());
                cursor.consumeArg();
                continue;
            } else {
                return result;
            }
        }

        // Attempt to parse the switch. The parseOneSwitch() throws an exception if something goes wrong, but returns NULL if
        // there's no switch to parse.
        try {
            parseOneSwitch(cursor, result);
        } catch (const std::runtime_error&) {
            if (skipUnknownSwitches_) {
                result.skip(cursor.location());
                cursor.consumeArg();
                continue;
            } else {
                throw;
            }
        }
    }

    return result;                                      // reached end of program arguments
}

SAWYER_EXPORT bool
Parser::parseOneSwitch(Cursor &cursor, ParserResult &result) {
    ASSERT_require(cursor.atArgBegin());
    Optional<std::runtime_error> saved_error;

    // Single long switch
    ParsedValues values;
    if (const Switch *sw = parseLongSwitch(cursor, values, saved_error /*out*/)) {
        ASSERT_require(cursor.atArgBegin() || cursor.atEnd());
        result.insertValuesForSwitch(values, this, sw);
        return true;
    }

    if (!shortMayNestle_) {
        // Single short switch
        if (const Switch *sw = parseShortSwitch(cursor, values, saved_error, shortMayNestle_)) {
            ASSERT_require(cursor.atArgBegin() || cursor.atEnd());
            result.insertValuesForSwitch(values, this, sw);
            return true;
        }
    } else {
        // Or multiple short switches.  If short switches are nestled, then the result is affected only if all the nesltled
        // switches can be parsed.
        typedef std::pair<const Switch*, ParsedValues> SwitchValues;
        std::list<SwitchValues> valuesBySwitch;         // values for each nestled switch that was parsed
        ExcursionGuard guard(cursor);
        while (guard.startingLocation().idx == cursor.location().idx) {
            if (const Switch *sw = parseShortSwitch(cursor, values, saved_error, shortMayNestle_)) {
                valuesBySwitch.push_back(SwitchValues(sw, values));
                values.clear();
            } else {
                break;
            }
        }
        if (!valuesBySwitch.empty() && (cursor.atArgBegin() || cursor.atEnd())) {
            BOOST_FOREACH (SwitchValues &svpair, valuesBySwitch)
                result.insertValuesForSwitch(svpair.second, this, svpair.first);
            guard.cancel();
            return true;
        }
    }

    // Throw or return zero?
    if (saved_error)
        throw *saved_error;                             // found at least one switch but couldn't ever parse arguments
    if (apparentSwitch(cursor))
        throw std::runtime_error("unrecognized switch: " + cursor.arg());
    return false;
}

static bool decreasingLength(const std::string &a, const std::string &b) {
    return a.size() < b.size();
}

SAWYER_EXPORT const Switch*
Parser::parseLongSwitch(Cursor &cursor, ParsedValues &parsedValues,
                        Optional<std::runtime_error> &saved_error) {
    ASSERT_require(cursor.atArgBegin());
    BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
        ParsingProperties sgProps = sg.properties().inherit(properties_);
        BOOST_FOREACH (const Switch &sw, sg.switches()) {
            ParsingProperties swProps = sw.properties().inherit(sgProps);
            std::vector<std::string> longNames = sw.longNames();
            std::sort(longNames.begin(), longNames.end(), decreasingLength);
            BOOST_FOREACH (const std::string &longName, longNames) {
                ExcursionGuard guard(cursor);
                Location switchLocation = cursor.location();
                if (sw.matchLongName(cursor, swProps, longName)) {
                    const std::string switchString = cursor.substr(switchLocation);
                    try {
                        ParsedValues pvals;
                        sw.matchLongArguments(switchString, cursor, swProps, pvals /*out*/);
                        ASSERT_require2(cursor.atArgBegin() || cursor.atEnd(), "invalid cursor position after long arguments");
                        BOOST_FOREACH (ParsedValue &pv, pvals)
                            pv.switchInfo(sw.key(), switchLocation, switchString);
                        parsedValues.insert(parsedValues.end(), pvals.begin(), pvals.end()); // may throw
                        guard.cancel();
                        return &sw;
                    } catch (const std::runtime_error &e) {
                        saved_error = e;
                    }
                }
            }
        }
    }
    return NULL;
}

SAWYER_EXPORT const Switch*
Parser::parseShortSwitch(Cursor &cursor, ParsedValues &parsedValues,
                         Optional<std::runtime_error> &saved_error, bool mayNestle) {
    ASSERT_require(mayNestle || cursor.atArgBegin());
    BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
        ParsingProperties sgProps = sg.properties().inherit(properties_);
        BOOST_FOREACH (const Switch &sw, sg.switches()) {
            ExcursionGuard guard(cursor);
            ParsingProperties swProps = sw.properties().inherit(sgProps);
            Location switchLocation = cursor.location();
            std::string switchString;
            if (sw.matchShortName(cursor, swProps, switchString /*out*/)) {
                try {
                    ParsedValues pvals;
                    sw.matchShortArguments(switchString, cursor, swProps, pvals /*out*/, mayNestle);
                    ASSERT_require(mayNestle || cursor.atArgBegin() || cursor.atArgEnd());
                    BOOST_FOREACH (ParsedValue &pv, pvals)
                        pv.switchInfo(sw.key(), switchLocation, switchString);
                    parsedValues.insert(parsedValues.end(), pvals.begin(), pvals.end()); // may throw
                    guard.cancel();
                    return &sw;
                } catch (const std::runtime_error &e) {
                    saved_error = e;
                }
            }
        }
    }
    return NULL;
}
    
SAWYER_EXPORT bool
Parser::apparentSwitch(const Cursor &cursor) const {
    BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
        ParsingProperties sgProps = sg.properties().inherit(properties_);
        BOOST_FOREACH (const Switch &sw, sg.switches()) {
            ParsingProperties swProps = sw.properties().inherit(sgProps);
            BOOST_FOREACH (const std::string &prefix, swProps.longPrefixes) {
                if (!prefix.empty() && boost::starts_with(cursor.arg(), prefix) && cursor.arg().size() > prefix.size())
                    return true;
            }
            BOOST_FOREACH (const std::string &prefix, swProps.shortPrefixes) {
                if (!prefix.empty() && boost::starts_with(cursor.arg(), prefix) && cursor.arg().size() > prefix.size())
                    return true;
            }
        }
    }
    return false;
}

// Read a text file to obtain command line arguments which are returned.
SAWYER_EXPORT std::vector<std::string>
Parser::readArgsFromFile(const std::string &filename) {
    std::vector<std::string> retval;
#include <sawyer/WarningsOff.h>                         // turn off warnings for fopen and strerror in MVC
    struct FileGuard {
        FILE *f;
        FileGuard(FILE *f): f(f) {}
        ~FileGuard() {
            if (f)
                fclose(f);
        }
    } file(fopen(filename.c_str(), "r"));
    if (NULL==file.f)
        throw std::runtime_error("failed to open file \"" + filename + "\": " + strerror(errno));
#include <sawyer/WarningsRestore.h>

    unsigned nlines = 0;
    while (1) {
        ++nlines;
        std::string line = readOneLine(file.f);
        if (line.empty())
            break;
        boost::trim(line);
        size_t nchars = line.size();
        if (line.empty() || '#'==line[0])
            continue;
        char inQuote = '\0';
        std::string word;

        for (size_t i=0; i<nchars; ++i) {
            char ch = line[i];
            if ('\''==ch || '"'==ch) {
                if (ch==inQuote) {
                    inQuote = '\0';
                } else if (!inQuote) {
                    inQuote = ch;
                } else {
                    word += ch;
                }
            } else if ('\\'==ch && i+1<nchars && (strchr("'\"\\", line[i+1]) || isspace(line[i+1]))) {
                word += line[++i];
            } else if (isspace(ch) && !inQuote) {
                while (i+1<nchars && isspace(line[i+1]))
                    ++i;
                retval.push_back(word);
                word = "";
            } else {
                word += ch;
            }
        }
        retval.push_back(word);

        if (inQuote) {
            std::ostringstream ss;
            ss <<"unterminated quote at line " <<nlines <<" in " <<filename;
            throw std::runtime_error(ss.str());
        }
    }
    return retval;
}

SAWYER_EXPORT const std::string&
Parser::programName() const {
    if (programName_.empty()) {
        Optional<std::string> s = Message::Prefix::instance()->programName();
        if (s)
            programName_ = *s;
    }
    return programName_;
}

SAWYER_EXPORT Parser&
Parser::version(const std::string &versionString, const std::string &dateString) {
    versionString_ = versionString;
    dateString_ = dateString;
    return *this;
}

SAWYER_EXPORT std::pair<std::string, std::string>
Parser::version() const {
    if (dateString_.empty()) {
        time_t now = time(NULL);
#include <sawyer/WarningsOff.h>
        if (const struct tm *tm_static = localtime(&now)) { // localtime_r not avail on Windows
#include <sawyer/WarningsRestore.h>
            static const char *month[] = {"January", "February", "March", "April", "May", "June", "July",
                                          "August", "September", "October", "November", "December"};
            dateString_ = std::string(month[tm_static->tm_mon]) + " " + toString(1900+tm_static->tm_year);
        }
    }
    return std::make_pair(versionString_, dateString_);
}

SAWYER_EXPORT Parser&
Parser::chapter(int chapterNumber, const std::string &chapterName) {
    int cn = chapterNumber_ = chapterNumber < 1 || chapterNumber > 9 ? 1 : chapterNumber;
    if (chapterName.empty()) {
        static const char *chapter[] = {
            "",                                         // 0
            "User Commands",                            // 1
            "System Calls",                             // 2
            "Libraries",                                // 3
            "Devices",                                  // 4
            "File Formats",                             // 5
            "Games",                                    // 6
            "Miscellaneous",                            // 7
            "System Administration",                    // 8
            "Documentation"                             // 9
        };
        chapterName_ = chapter[cn];
    } else {
        chapterName_ = chapterName;
    }
    return *this;
}

SAWYER_EXPORT std::pair<int, std::string>
Parser::chapter() const {
    return std::make_pair(chapterNumber_, chapterName_);
}

SAWYER_EXPORT Parser&
Parser::doc(const std::string &sectionName, const std::string &docKey, const std::string &text) {
    checkMarkup(text);
    sectionOrder_.insert(docKey, sectionName);
    sectionDoc_.insert(boost::to_lower_copy(sectionName), text);
    return *this;
}

SAWYER_EXPORT std::vector<std::string>
Parser::docSections() const {
    std::vector<std::string> retval;
    BOOST_FOREACH (const std::string &key, sectionDoc_.keys())
        retval.push_back(key);
    return retval;
}

// @s{NAME} where NAME is either a long or short switch name without prefix.
typedef Container::Map<std::string, std::string> PreferredPrefixes; // maps switch names to their best prefixes

class SwitchTag: public Markup::Tag {
    PreferredPrefixes preferredPrefixes_;
    std::string bestShortPrefix_;                       // short prefix if the switch name is not recognized
    std::string bestLongPrefix_;                        // long prefix if the switch name is not recognized
protected:
    SwitchTag(const PreferredPrefixes &known, const std::string &bestShort, const std::string &bestLong)
        : Markup::Tag(Markup::SPANNING, "switch", Markup::SPANNING),
          preferredPrefixes_(known), bestShortPrefix_(bestShort), bestLongPrefix_(bestLong) {}
public:
    typedef SharedPointer<SwitchTag> Ptr;
    static Ptr instance(const PreferredPrefixes &known, const std::string &bestShort, const std::string &bestLong) {
        return Ptr(new SwitchTag(known, bestShort, bestLong));
    }
    virtual Markup::Content::Ptr eval(const Markup::TagArgs &args) /*override*/ {
        using namespace Markup;
        ASSERT_require(1==args.size());
        std::string raw = args.front()->asText();
        PreferredPrefixes::NodeIterator i = preferredPrefixes_.find(raw);
        if (i==preferredPrefixes_.nodes().end()) {
            if (1==raw.size()) {
                raw = bestShortPrefix_ + raw;
            } else {
                raw = bestLongPrefix_ + raw;
            }
        } else {
            raw = i->value() + raw;
        }
        TagInstance::Ptr nulltag = TagInstance::instance(NullTag::instance(raw));
        Content::Ptr retval = Content::instance();
        retval->append(nulltag);
        return retval;
    }
};

// @seeAlso is replaced by the list @man references that have been processed so far.
class SeeAlsoTag: public Markup::Tag {
public:
    typedef SharedPointer<SeeAlsoTag> Ptr;
    typedef Container::Map<std::string, Markup::Content::Ptr> SeeAlso;
private:
    SeeAlso seeAlso_;
protected:
    SeeAlsoTag(): Markup::Tag(Markup::DIVIDING, "seeAlso") {}
public:
    static Ptr instance() { return Ptr(new SeeAlsoTag); }
    void insert(const std::string &name, const Markup::Content::Ptr &content) {
        seeAlso_.insert(name, content);
    }
    virtual Markup::Content::Ptr eval(const Markup::TagArgs &args) /*override*/ {
        using namespace Markup;
        ASSERT_require(0==args.size());
        Content::Ptr retval = Content::instance();
        for (SeeAlso::ValueIterator sai=seeAlso_.values().begin(); sai!=seeAlso_.values().end(); ++sai) {
            if (sai!=seeAlso_.values().begin())
                retval->append(", ");
            retval->append(*sai);
        }
        return retval;
    }
};

// @man{PAGE}{CHAPTER} converted to @em{PAGE}(CHAPTER) to cite Unix manual pages.
class ManTag: public Markup::Tag {
    SeeAlsoTag::Ptr seeAlso_;
protected:
    ManTag(const SeeAlsoTag::Ptr &seeAlso)
        : Markup::Tag(Markup::SPANNING, "man", Markup::SPANNING, Markup::SPANNING), seeAlso_(seeAlso) {}
public:
    typedef SharedPointer<ManTag> Ptr;
    static Ptr instance(const SeeAlsoTag::Ptr &seeAlso) { return Ptr(new ManTag(seeAlso)); }
    virtual Markup::Content::Ptr eval(const Markup::TagArgs &args) /*override*/ {
        using namespace Markup;
        ASSERT_require(2==args.size());
        Content::Ptr retval = Content::instance();
        retval->append(TagInstance::instance(EmphasisTag::instance(), args[0]));
        retval->append("(");
        retval->append(args[1]);
        retval->append(")");
        seeAlso_->insert(args[0]->asText(), retval);
        return retval;
    }
};

// @prop{KEY} is replaced with the property string stored for KEY
class PropTag: public Markup::Tag {
    Container::Map<std::string, std::string> values_;
protected:
    PropTag(): Markup::Tag(Markup::SPANNING, "prop", Markup::SPANNING) {}
public:
    typedef SharedPointer<PropTag> Ptr;
    static Ptr instance() { return Ptr(new PropTag); }
    Ptr with(const std::string &key, const std::string &value) {
        values_.insert(key, value);
        return sharedFromThis().dynamicCast<PropTag>();
    }
    virtual Markup::Content::Ptr eval(const Markup::TagArgs &args) /*overload*/ {
        using namespace Markup;
        ASSERT_require(1==args.size());
        std::string key = args.front()->asText();
        Content::Ptr retval = Content::instance();
        retval->append(values_.getOrDefault(key));
        return retval;
    }
};

struct SwitchDoc {
    std::string sortMajor, sortMinor;                   // sorting keys
    std::string groupKey;
    std::string docString;
    SwitchDoc(const std::string &sortMajor, const std::string &sortMinor, const std::string &groupKey,
              const std::string &docString)
        : sortMajor(sortMajor), sortMinor(sortMinor), groupKey(groupKey), docString(docString) {}
};

static bool
sortSwitchDoc(const SwitchDoc &a, const SwitchDoc &b) {
    if (a.sortMajor != b.sortMajor)
        return a.sortMajor < b.sortMajor;
    return a.sortMinor < b.sortMinor;
}

static std::string
nextSortKey() {
    static size_t ncalls = 0;
    static char buf[9];
    for (size_t i=sizeof(buf)-1, n=ncalls++; i>0; --i, n/=26)
        buf[i-1] = 'a' + (n % 26);
    buf[sizeof(buf)-1] = '\0';
    return buf;
}

// Returns documentation for all the switches
SAWYER_EXPORT std::string
Parser::docForSwitches() const {
    // Accumulate and sort documentation for each switch
    Container::Map<std::string /*groupkey*/, std::string /*description*/> groupDescriptions;
    Container::Map<std::string /*groupkey*/, std::string /*title*/> groupTitles;
    std::string notDocumented = "Not documented.";
    std::vector<SwitchDoc> switchDocs;
    BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
        std::string groupKey = sg.docKey().empty() ? boost::to_lower_copy(sg.name()) : sg.docKey();
        std::string sortMajor;
        switch (switchGroupOrder_) {
            case DOCKEY_ORDER:          sortMajor = groupKey;           break;
            case INSERTION_ORDER:       sortMajor = nextSortKey();      break;
        }

        // Switch group title and doc string. When multiple groups have the same key, the title is the name of the first such
        // group and the doc string is the concatenation from all such groups.
        if (!sg.name().empty())
            groupTitles.insertMaybe(groupKey, sg.name());
        if (!sg.doc().empty()) {
            std::string s = groupDescriptions.getOptional(groupKey).orDefault();
            s += (s.empty() ? "" : "\n\n") + sg.doc();
            groupDescriptions.insert(groupKey, s);
        }
        
        // Accumulate doc strings for the switches in this group.
        BOOST_FOREACH (const Switch &sw, sg.switches()) {
            if (sw.hidden())
                continue;
            std::string switchKey = sw.docKey().empty() ? boost::to_lower_copy(sw.key()) : sw.docKey();
            std::string sortMinor;
            switch (sg.switchOrder()) {
                case DOCKEY_ORDER:      sortMinor = switchKey;          break;
                case INSERTION_ORDER:   sortMinor = nextSortKey();      break;
            }
            std::string markup = "@named{" + sw.synopsis() + "}{" + (sw.doc().empty() ? notDocumented : sw.doc()) + "}\n";
            switchDocs.push_back(SwitchDoc(sortMajor, sortMinor, groupKey, markup));
        }
    }
    std::sort(switchDocs.begin(), switchDocs.end(), sortSwitchDoc);
    
    // Generate the result
    std::string result, prevGroupKey, closeSection;
    BOOST_FOREACH (const SwitchDoc &switchDoc, switchDocs) {
        if (switchDoc.groupKey != prevGroupKey) {
            std::string groupTitle = groupTitles.getOptional(switchDoc.groupKey).orElse("");
            std::string groupDesc = groupDescriptions.getOptional(switchDoc.groupKey).orElse("");
            result += closeSection;
            if (!groupTitle.empty()) {
                result += "@section{" + groupTitle + "}{";
                closeSection = "}\n";
            } else {
                closeSection = "";
            }
            result += groupDesc + "\n\n";
            prevGroupKey = switchDoc.groupKey;
        }
        result += switchDoc.docString;
    }
    result += closeSection;
    return result;
}

SAWYER_EXPORT std::string
Parser::docForSection(const std::string &sectionName) const {
    std::string docKey = boost::to_lower_copy(sectionName);
    StringStringMap::ConstNodeIterator section = sectionDoc_.find(docKey);
    std::string doc = section == sectionDoc_.nodes().end() ? std::string() : section->value();
    if (0==docKey.compare("name")) {
        if (doc.empty())
            doc = programName() + " - " + (purpose_.empty() ? std::string("Undocumented") : purpose_);
    } else if (0==docKey.compare("synopsis")) {
        if (doc.empty())
            doc = programName() + " [@v{switches}...]\n";
    } else if (0==docKey.compare("switches")) {
        doc += "\n\n" + docForSwitches();
    } else if (0==docKey.compare("see also")) {
        doc += "\n\n@seeAlso";
    }
    
    return doc;
}

// Returns a map that lists all known switches and their preferred prefix
SAWYER_EXPORT void
Parser::preferredSwitchPrefixes(Container::Map<std::string, std::string> &prefixMap /*out*/) const {
    BOOST_FOREACH (const SwitchGroup &sg, switchGroups_) {
        ParsingProperties sgProps = sg.properties().inherit(properties_);
        BOOST_FOREACH (const Switch &sw, sg.switches()) {
            ParsingProperties swProps = sw.properties().inherit(sgProps);
            if (!swProps.longPrefixes.empty()) {
                const std::string &prefix = swProps.longPrefixes.front();
                BOOST_FOREACH (const std::string &name, sw.longNames())
                    prefixMap.insert(name, prefix);
            }
            if (!swProps.shortPrefixes.empty()) {
                const std::string &prefix = swProps.shortPrefixes.front();
                BOOST_FOREACH (char name, sw.shortNames())
                    prefixMap.insert(std::string(1, name), prefix);
            }
        }
    }
}

// Obtain the documentation markup for this parser.
SAWYER_EXPORT std::string
Parser::documentationMarkup() const {
    std::set<std::string> created;                      // sections that we've created

    // The man pages starts with some sections that are always present in the same order.
    std::string doc = "@section{Name}{" + docForSection("name") + "}\n" +
                      "@section{Synopsis}{" + docForSection("synopsis") + "}\n" +
                      "@section{Description}{" + docForSection("description") + "}\n" +
                      "@section{Switches}{" + docForSection("switches") + "}\n";
    created.insert("name");
    created.insert("synopsis");
    created.insert("description");
    created.insert("switches");

    // Append user-defined sections
    BOOST_FOREACH (const std::string &sectionName, sectionOrder_.values()) {
        if (created.insert(boost::to_lower_copy(sectionName)).second)
            doc += "@section{" + sectionName + "}{" + docForSection(sectionName) + "}\n";
    }

    // This section is always at the bottom unless the user forces it elsewhere.
    if (created.insert("see also").second)
        doc += "@section{See Also}{" + docForSection("see also") + "}\n";
    return doc;
}

SAWYER_EXPORT Markup::ParserResult
Parser::parseDocumentation() const {
    return parseDocumentation(documentationMarkup());
}

SAWYER_EXPORT Markup::ParserResult
Parser::parseDocumentation(const std::string &docstring) const {
    // The @s tag for expanding switch names from "foo" to "--foo", or whatever is appropriate
    Container::Map<std::string, std::string> prefixes;
    preferredSwitchPrefixes(prefixes /*out*/);
    std::string bestShort = properties_.shortPrefixes.empty() ?
                            std::string("-") :
                            properties_.shortPrefixes.front();
    std::string bestLong = properties_.longPrefixes.empty() ?
                           std::string("--") :
                           properties_.longPrefixes.front();

    // Make some properties available in the markup
    PropTag::Ptr properties = PropTag::instance();
    properties
        ->with("inclusionPrefix", inclusionPrefixes_.empty() ? std::string() : inclusionPrefixes_.front())
        ->with("terminationSwitch", terminationSwitches_.empty() ? std::string() : terminationSwitches_.front())
        ->with("programName", programName())
        ->with("purpose", purpose())
        ->with("versionString", version().first)
        ->with("versionDate", version().second)
        ->with("chapterNumber", toString(chapter().first))
        ->with("chapterName", chapter().second);

    // This tag decl will accumulate all the @man references
    SeeAlsoTag::Ptr seeAlso = SeeAlsoTag::instance();

    Markup::Parser mp;
    mp.registerTag(SwitchTag::instance(prefixes, bestShort, bestLong), "s");
    mp.registerTag(ManTag::instance(seeAlso), "man");
    mp.registerTag(seeAlso, "seeAlso");
    mp.registerTag(properties, "prop");

    return mp.parse(docstring);
}

SAWYER_EXPORT std::string
Parser::podDocumentation() const {
    Markup::ParserResult doc = parseDocumentation();
    std::ostringstream ss;
    doc.emit(ss, Markup::PodFormatter::instance());
    return ss.str();
}

SAWYER_EXPORT std::string
Parser::manDocumentation() const {
    Markup::PodFormatter::Ptr podder = Markup::PodFormatter::instance();
    podder->title(programName(), toString(chapter().first), chapter().second);
    podder->version(version().first, version().second);
    return podder->toNroff(parseDocumentation());
}

SAWYER_EXPORT void
Parser::emitDocumentationToPager() const {
    Markup::PodFormatter::Ptr podder = Markup::PodFormatter::instance();
    podder->title(programName(), toString(chapter().first), chapter().second);
    podder->version(version().first, version().second);
    podder->emit(parseDocumentation());
}

SAWYER_EXPORT void
checkMarkup(const std::string &s) {
    Markup::Parser mp;

    // Same as for Parser::parseDocument except just stubbed out to test for syntax
    Container::Map<std::string, std::string> prefixes;
    mp.registerTag(SwitchTag::instance(prefixes, "-", "--"), "s");
    SeeAlsoTag::Ptr seeAlso = SeeAlsoTag::instance();
    mp.registerTag(ManTag::instance(seeAlso), "man");
    mp.registerTag(seeAlso, "seeAlso");
    mp.registerTag(PropTag::instance(), "prop");

    mp.parse("@section{X}{"+s+"}");             // throws on error
}

} // namespace
} // namespace
