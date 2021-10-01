#ifndef matrixTools_H
#define matrixTools_H

#include <Sawyer/CommandLine.h>
#include <Sawyer/Database.h>
#include <time.h>
#include <string>

enum class Format {
    PLAIN,
    YAML
};

struct Dependency {
    std::string name;
    std::string value;
    std::string comment;
    bool enabled = false;
    bool supported = false;
};

using DependencyList = std::vector<Dependency>;

// Insert the --database / -D switch into the switch group so that the result is stored in 'uri'
void insertDatabaseSwitch(Sawyer::CommandLine::SwitchGroup&, std::string &uri);

// Insert the --format / -F switch.
void insertOutputFormatSwitch(Sawyer::CommandLine::SwitchGroup&, Format&);

// Convert Unix epoch time to human readable time in the local timezone.
std::string timeToLocal(time_t);

// if the version is a SHA1, then return an abbreviated version.
std::string abbreviatedVersion(const std::string&);

// Given a point in time, return a string like "about 5 days ago". This also works for future time points ("about 5 days in the
// future").
std::string approximateAge(time_t point);

// The list of dependency columns that must be returned when calling loadDependencies
std::string dependencyColumns();

// Load dependency info from database.
DependencyList loadDependencies(Sawyer::Database::Statement);

#endif
