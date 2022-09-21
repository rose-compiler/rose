#ifndef matrixTools_H
#define matrixTools_H

#include <Rose/BitFlags.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <set>
#include <time.h>
#include <string>

enum class Format {
    PLAIN       = 0x00000001,
    YAML        = 0x00000002,
    HTML        = 0x00000004,
    CSV         = 0x00000008,
    SHELL       = 0x00000010
};

using FormatFlags = Rose::BitFlags<Format>;

// Information about dependencies, such as "tup 0.7.8 is enabled only on ubuntu-20.04 when the buid type is tup"
class Dependency {
public:
    // The right hand side of dependency constraints.
    struct ConstraintRhs {
        std::string comparison;                         // type of comparison to perform
        std::string value;                              // value to compare against

        ConstraintRhs() {}
        ConstraintRhs(const std::string &comparison, const std::string &value)
            : comparison(comparison), value(value) {}

        bool operator==(const ConstraintRhs &other) const {
            return comparison == other.comparison && value == other.value;
        }
    };

    // The LHS is a dependency name such as "build" and the value is an vecotr of comparisons between build's value and a
    // provided RHS value.
    using Constraints = Sawyer::Container::Map<std::string /*lhs*/, std::vector<ConstraintRhs>>;

public:
    std::string name;                                   //  "tup"
    std::string value;                                  //  "0.7.8"
    std::string comment;                                //  "Has some problems on RedHat"
    bool enabled = false;                               //  true: this dependency should be tested
    bool supported = false;                             //  false: not officially supported by ROSE
    std::set<std::string> osNames;                      //  "ubuntu:2004 ubuntu:1804"
    Constraints constraints;                            //  build=tup, languages=binaries
};

// Dependencies in some order
using DependencyList = std::vector<Dependency>;

// The list of dependency columns that must be returned when calling loadDependencies
std::string dependencyColumns();

// Load dependency info from database. The statement must select the columns returned by dependencyColumns starting at index 0,
// but but it may return additional columns if necessary.
DependencyList loadDependencies(Sawyer::Database::Statement);

// Update a dependency. The name and value must already exist.
void updateDependency(Sawyer::Database::Connection, const Dependency&);

// Insert the --database / -D switch into the switch group so that the result is stored in 'uri'
void insertDatabaseSwitch(Sawyer::CommandLine::SwitchGroup&, std::string &uri);

// Insert the --format / -F switch.
void insertOutputFormatSwitch(Sawyer::CommandLine::SwitchGroup&, Format&, FormatFlags);

// Convert Unix epoch time to human readable time in the local timezone.
std::string timeToLocal(time_t);

// Convert Unix epoch time to human readable time in the GMT timezone.
std::string timeToGmt(time_t);

// if the version is a SHA1, then return an abbreviated version.
std::string abbreviatedVersion(const std::string&);

// Given a point in time, return a string like "about 5 days ago". This also works for future time points ("about 5 days in the
// future").
std::string approximateAge(time_t point);

// Convert an output format to a table format.
Rose::FormattedTable::Format tableFormat(Format);

// Split a string into words at the whitespace, trimming all whitespace from each word and not returning any empty words.
std::vector<std::string> splitIntoWords(const std::string&);

// Open a connection to a database, or fail with some info about how to specify a database name
Sawyer::Database::Connection connectToDatabase(const std::string &uri, Sawyer::Message::Facility&);

// Table cell format with a green background
const Rose::FormattedTable::CellProperties& goodCell();

// Table cell format with a red background
const Rose::FormattedTable::CellProperties& badCell();

// Table chell format that centers the text
const Rose::FormattedTable::CellProperties& centered();

#endif
