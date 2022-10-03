#include "matrixTools.h"

#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/SplitJoin.h>
#include <Sawyer/Database.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

DB::Connection
connectToDatabase(const std::string &uri, Sawyer::Message::Facility &mlog) {
    if (uri.empty()) {
        mlog[FATAL] <<"no database specified\n";
        mlog[INFO] <<"You need to use the --database switch, or set your ROSE_MATRIX_DATABASE environment\n"
                   <<"variable. See the \"Testing\" section of https://toc.rosecompiler.org for the proper\n"
                   <<"setting.\n";
        exit(1);
    }
    return DB::Connection::fromUri(uri);
}

void
insertDatabaseSwitch(Sawyer::CommandLine::SwitchGroup &sg, std::string &uri) {
    using namespace Sawyer::CommandLine;


    if (uri.empty()) {
#ifdef DEFAULT_DATABASE
        uri = DEFAULT_DATABASE;
#endif
        if (const char *s = getenv("ROSE_MATRIX_DATABASE"))
            uri = s;
    }

    sg.insert(Switch("database", 'D')
              .argument("uri", anyParser(uri))
              .doc("URI specifying which database to use. This switch overrides the ROSE_MATRIX_DATABASE environment variable. " +
                   Sawyer::Database::Connection::uriDocString()));
}

void
insertOutputFormatSwitch(Sawyer::CommandLine::SwitchGroup &sg, Format &fmt, FormatFlags enabled) {
    using namespace Sawyer::CommandLine;

    auto ep = enumParser(fmt);
    std::string doc = "Specifies how to format the results. The choices are:";
    if (enabled.isSet(Format::PLAIN)) {
        ep->with("plain", Format::PLAIN);
        doc += "@named{plain}{Plain text, human-readable format.}";
    }
    if (enabled.isSet(Format::YAML)) {
        ep->with("yaml", Format::YAML);
        doc += "@named{yaml}{Structured YAML output.}";
    }
    if (enabled.isSet(Format::HTML)) {
        ep->with("html", Format::HTML);
        doc += "@named{html}{HTML web page.}";
    }
    if (enabled.isSet(Format::CSV)) {
        ep->with("csv", Format::CSV);
        doc += "@named{csv}{Comma separated values per RFC 4180.}";
    }
    if (enabled.isSet(Format::SHELL)) {
        ep->with("shell", Format::SHELL);
        doc += "@named{shell}{Shell-script fiendly output. One row of data per line with Unix line endings; columns are "
               "separated from one another with a single horizontal tab character; data that contains line feed characters, "
               "horizontal tab characters, or shell meta characters are escaped so that those characters don't appear naked "
               "in the output; the escaping is C-like using the shell's dollar-single-quote mechanism ($'...').}";
    }

    sg.insert(Switch("format", 'F')
              .argument("style", ep)
              .doc(doc));
}

std::string
timeToLocal(time_t t) {
    struct tm tm;
    std::string tz;
    localtime_r(&t, &tm);
    tz = tm.tm_zone;
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d %s")
            % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec
            % tz).str();
}

std::string
timeToGmt(time_t t) {
    struct tm tm;
    std::string tz;
    gmtime_r(&t, &tm);
    tz = tm.tm_zone;
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d %s")
            % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec
            % tz).str();
}

std::string
abbreviatedVersion(const std::string &s) {
    boost::regex sha1("([0-9a-f]{40})(\\+local)?");

    boost::smatch found;
    if (boost::regex_match(s, found, sha1)) {
        return found.str(1).substr(0, 8) + found.str(2);
    } else {
        return s;
    }
}

std::string
approximateAge(time_t t) {
    const time_t now = time(NULL);
    const time_t age = t <= now ? now - t : t - now;
    const bool reversed = t > now;
    static const time_t DAYS = 86400;

    std::string retval;
    if (age < 60) {
        retval = "a few seconds";
    } else if (age < 3600) {
        const time_t nMinutes = (age + 30) / 60;
        retval = "about " + Rose::StringUtility::plural(nMinutes, "minutes");
    } else if (age < 1*DAYS) {
        const time_t nHours = (age + 1800) / 3600;
        retval = "about " + Rose::StringUtility::plural(nHours, "hours");
    } else if (age < 7*DAYS) {
        const time_t nDays = (age + DAYS/2) / DAYS;
        retval = "about " + Rose::StringUtility::plural(nDays, "days");
    } else if (age < 60*DAYS) {
        const time_t nWeeks = (age + 7*DAYS/2) / (7*DAYS);
        retval = "about " + Rose::StringUtility::plural(nWeeks, "weeks");
    } else if (age < 365*DAYS) {
        const time_t nMonths = (age + 30*DAYS/2) / (30*DAYS);
        retval = "about " + Rose::StringUtility::plural(nMonths, "months");
    } else {
        const time_t nYears = (age + 365*DAYS/2) / (365*DAYS);
        retval = "about " + Rose::StringUtility::plural(nYears, "years");
    }

    retval += reversed ? " in the future" : " ago";
    return retval;
}

std::string dependencyColumns() {
    //      0     1      2        3        4          5
    return "name, value, comment, enabled, supported, restrictions";
}

std::vector<std::string>
splitIntoWords(const std::string &s) {
    std::vector<std::string> words;
    std::string word;
    for (char ch: s) {
        if (::isspace(ch)) {
            if (!word.empty()) {
                words.push_back(word);
                word = "";
            }
        } else {
            word += ch;
        }
    }
    if (!word.empty())
        words.push_back(word);
    return words;
}

// Load depencency info
DependencyList
loadDependencies(DB::Statement stmt) {
    DependencyList deps;
    boost::regex osRe("([a-zA-Z]\\w*):(\\d+(\\.\\d+)*)");    // OS_NAME ':' VERSION_WOUT_DOTS
    boost::regex nameOpValueRe("([a-zA-Z]\\w*)([=/~^])(.*)"); // NAME OPERATOR VALUE

    for (auto row: stmt) {
        deps.push_back(Dependency());
        Dependency &d = deps.back();

        d.name = row.get<std::string>(0).orElse("null");
        d.value = row.get<std::string>(1).orElse("null");
        d.comment = row.get<std::string>(2).orElse("");
        d.enabled = row.get<bool>(3).orElse(false);
        d.supported = row.get<bool>(4).orElse(false);

        if (auto restrictions = row.get<std::string>(5)) {
            boost::smatch found;
            std::vector<std::string> parts = splitIntoWords(*restrictions);
            for (const std::string &part: parts) {
                if (boost::regex_match(part, found, osRe)) {
                    d.osNames.insert(part);
                } else if (boost::regex_match(part, found, nameOpValueRe)) {
                    const std::string otherDepName = found.str(1);
                    const std::string relationship = found.str(2);
                    const std::string otherValue = found.str(3);
                    d.constraints.insertMaybeDefault(otherDepName)
                        .push_back(Dependency::ConstraintRhs(relationship, otherValue));
                } else {
                    throw std::runtime_error("error in constraint \"" + StringUtility::cEscape(part) + "\"" +
                                             " for dependency \"" + StringUtility::cEscape(d.name) + "\"" +
                                             " value \"" + StringUtility::cEscape(d.value) + "\n");
                }
            }
        }
    }
    return deps;
}

// Update an existing dependency
void
updateDependency(DB::Connection db, const Dependency &dep) {
#ifndef NDEBUG
    bool exists = 1 == (db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
                        .bind("name", dep.name)
                        .bind("value", dep.value)
                        .get<size_t>().get());
    ASSERT_require(exists);
#endif

    std::string r;
    for (const std::string &os: dep.osNames)
        r += (r.empty() ? "" : " ") + os;
    for (const Dependency::Constraints::Node &node: dep.constraints.nodes()) {
        for (const Dependency::ConstraintRhs &rhs: node.value())
            r += (r.empty() ? "" : " ") + node.key() + rhs.comparison + rhs.value;
    }

    db.stmt("update dependencies"
            " set enabled = ?enabled,"
            "     supported = ?supported,"
            "     comment = ?comment,"
            "     restrictions = ?restrictions"
            " where name = ?name and value = ?value")
        .bind("enabled", dep.enabled ? 1 : 0)
        .bind("supported", dep.supported ? 1 : 0)
        .bind("comment", dep.comment)
        .bind("restrictions", r)
        .bind("name", dep.name)
        .bind("value", dep.value)
        .run();
}

Rose::FormattedTable::Format
tableFormat(Format fmt) {
    switch (fmt) {
        case Format::YAML:                              // yaml is not a table format, so use plain
        case Format::PLAIN:
            return Rose::FormattedTable::Format::PLAIN;
        case Format::HTML:
            return Rose::FormattedTable::Format::HTML;
        case Format::CSV:
            return Rose::FormattedTable::Format::CSV;
        case Format::SHELL:
            return Rose::FormattedTable::Format::SHELL;
    }
    ASSERT_not_reachable("invalid output format");
}

const FormattedTable::CellProperties&
badCell() {
    static bool called;
    static FormattedTable::CellProperties p;
    if (!called) {
        p.foreground(Color::HSV_RED);
        called = true;
    }
    return p;
}

const FormattedTable::CellProperties&
goodCell() {
    static bool called;
    static FormattedTable::CellProperties p;
    if (!called) {
        p.foreground(Color::HSV_GREEN);
        called = true;
    }
    return p;
}

const FormattedTable::CellProperties&
centered() {
    static bool called;
    static FormattedTable::CellProperties p;
    if (!called) {
        p.alignment(FormattedTable::Alignment::CENTER);
        called = true;
    }
    return p;
}
