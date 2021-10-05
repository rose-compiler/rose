#include "matrixTools.h"

#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/SplitJoin.h>
#include <Sawyer/Database.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

namespace DB = Sawyer::Database;

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
    return "name, value, comment, enabled, supported, os_list";
}

// Load depencency info
DependencyList
loadDependencies(DB::Statement stmt) {
    DependencyList deps;
    for (auto row: stmt) {
        deps.push_back(Dependency{
                    .name = *row.get<std::string>(0),
                    .value = *row.get<std::string>(1),
                    .comment = row.get<std::string>(2).orElse(""),
                    .enabled = *row.get<bool>(3),
                    .supported = *row.get<bool>(4)});

        std::vector<std::string> osNames = Rose::StringUtility::split(" ", row.get<std::string>(5).orDefault());
        for (std::string &osName: osNames) {
            boost::trim(osName);
            if (boost::ends_with(osName, ","))
                osName = osName.substr(0, osName.size()-1);
        }
        osNames.erase(std::remove(osNames.begin(), osNames.end(), std::string()), osNames.end());
        deps.back().osNames = std::set<std::string>(osNames.begin(), osNames.end());
    }
    return deps;
}
