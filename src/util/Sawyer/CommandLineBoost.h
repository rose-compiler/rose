// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_CommandLine_Boost_H
#define Sawyer_CommandLine_Boost_H

#include <Sawyer/CommandLine.h>
#include <Sawyer/Sawyer.h>

namespace Sawyer {
namespace CommandLine {

/** Drop-in replacement to help boost users.
 *
 *  Some users use boost::program_options, but due to the complexity of the interface, they use only the most basic
 *  features. Essentially, they use boost to map switches like "--foo=bar" into a map containing the pair ("foo", "bar") both
 *  represented as <code>std::string</code>. Their source code then makes decisions and obtains switch values by querying them
 *  from this map and converting them to a non-string type that's stored in a C++ variable for later reference.
 *
 *  Sawyer::CommandLine can also operate in this "pull" mode although it's most often used in "push" mode, where the
 *  command-line parsing package is responsible for converting the string argument to a non-string type and storing it in a C++
 *  variable somewhere. */
namespace Boost {

/** Replacement for basic use of boost::program_options::value.
 *
 *  The equivalent mechanism in Sawyer is split between two class hierarchies: a class that represents parsing of strings into
 *  non-string types, and a class that represents storing of non-string values into C++ variables. This separation of concerns
 *  allows Sawyer to parse one type but store it in a variable of a different, compatible type.  However, most users of Sawyer
 *  see these classes as functions like @ref Sawyer::CommandLine::nonNegativeIntegerParser "nonNegativeIntegerParser" taking
 *  one argument: some integer reference in which to ultimately store the result. */
template<class T>
struct value {};

/**  Replacement for basic use of boost::program_options::options_description.
 *
 *   The equivalent mechanism in Sawyer is a @ref Sawyer::CommandLine::SwitchGroup "SwitchGroup" that can hold declarations for
 *   zero or more switches. */
struct options_description {
    /** The underlying Sawyer mechanism. */
    Sawyer::CommandLine::SwitchGroup sg;

    /** Construct an empty switch group without documentation. */
    options_description() {}

    /** Construct an empty switch group having a title. */
    explicit options_description(const std::string &title): sg(title) {}

    /** Declare a switch of specified type.
     *
     *  As with boost (but not Sawyer) the type of the switch value is wrapped up in a template class called @ref value.
     *
     *  There are often slightly different semantics possible depending on the declaration details, but all that is glossed
     *  over in this simple wrapper.
     *
     *  @{ */
    options_description& operator()(const std::string &switchName, const value<std::string>&, const std::string &doc);
    options_description& operator()(const std::string &switchName, const value<int>&, const std::string &doc);
    options_description& operator()(const std::string &switchName, const value<long int>&, const std::string &doc);
    options_description& operator()(const std::string &switchName, const value<std::vector<int> >&, const std::string &doc);
    options_description& operator()(const std::string &switchName, const value< std::vector<std::string> >&,
                                    const std::string &doc);
    options_description& operator()(const std::string &switchName, const std::string &doc);
    /** @} */

    /** Boost intermediate type for adding more switches.
     *
     *  Sawyer does not need an intermediate type for adding switches since it uses an "insert" method and has a dedicated
     *  @ref Sawyer::CommandLine::Switch "Switch" type to hold all the switch properties. */
    options_description& add_options() {
        return *this;
    }

    /** Insert other switches into this group.
     *
     *  Copies the @p other switches into this switch group without making any attempt to resolve conflicts when the @p other
     *  group has switches with the same name as this group.  If that happens, you might end up with one switch that parses a
     *  string and another that parses an integer, and depending on their order, the string declaration might hide the integer
     *  declarations since strings are a superset of integers. */
    options_description& add(const options_description &other);

    /** Print switch documentation.
     *
     *  Sawyer normally produces manpage-style documentation for an entire program rather than line-oriented documentation for
     *  just a set of switch declarations. So in order to achieve the desired limited affect, this wrapper instantiates a
     *  temporary parser that contains these switches and nothing else. */
    void print() const;
};

/** Print documentation for a few switches. */
std::ostream& operator<<(std::ostream &out, const options_description &x);

/** Wrapper around Sawyer's CommandLine class. */
struct command_line_parser {
    int argc;                                           /**< Argument count saved by c'tor to be available during parsing. */
    char **argv;                                        /**< Arguments saved by c'tor to be available during parsing. */
    Sawyer::CommandLine::Parser parser;                 /**< Wrapped parser. */

    /** Construct a parser.
     *
     *  The command-line is not actually parsed here -- its only saved by reference until @ref run is called. */
    command_line_parser(int argc, char *argv[]): argc(argc), argv(argv) {
        parser.errorStream(Sawyer::Message::mlog[Sawyer::Message::FATAL]); // use error messages instead of exceptions
    }

    /** Insert specified switch declarations.
     *
     *  @{ */
    command_line_parser& options(const options_description&);
    command_line_parser& options(const Sawyer::CommandLine::SwitchGroup&);
    /** @} */

    /** Add predefined switches.
     *
     *  The boost version of this method causes boost to capture values of switches that have not been declared. Although
     *  Sawyer is also capable of skipping over and accumulating arguments that look like switches but which have not been
     *  declared, doing so is fraught with danger.  Imagine the synchronization and missing code problems that would result if
     *  a source code compiler took this same approach! */
    command_line_parser& allow_unregistered() { return *this; }

    /** Parse command-line.
     *
     *  Parses the command line and return results. If the syntax is good, this also runs the "--help" switch if present. */
    Sawyer::CommandLine::ParserResult run() {
        return parser.parse(argc, argv).apply();
    }
};

/** Wrapper around parsed values.
 *
 *  Sawyer not only stores the parsed switch value, but also much information about where the value came from, it's original
 *  string form, etc. */
struct parsed_values {                                  // not a boost::program_options type
    Sawyer::CommandLine::ParsedValues pv;               /**< Wrapped ParsedValues. */

    /** Wrap nothing. */
    parsed_values() {}

    /** Wrap ParsedValues. */
    explicit parsed_values(const Sawyer::CommandLine::ParsedValues &pv): pv(pv) {}

    /** Convert parsed value to another type.
     *
     *  @{ */
    template<class T>
    T as() {
        return as_helper(T());
    }

private:
    template<class T>
    T as_helper(const T&) {
        ASSERT_forbid(pv.empty());
        return pv.back().as<T>();
    }

    template<class T>
    std::vector<T> as_helper(const std::vector<T>&) {
        std::vector<T> retval;
        BOOST_FOREACH (const Sawyer::CommandLine::ParsedValue &v, pv) {
            Sawyer::CommandLine::ListParser::ValueList elmts = v.as<Sawyer::CommandLine::ListParser::ValueList>();
            BOOST_FOREACH (const Sawyer::CommandLine::ParsedValue &elmt, elmts)
                retval.push_back(elmt.as<T>());
        }
        return retval;
    }
    /** @} */
    
};

/** Wrapper around ParserResult. */
struct variables_map {
    Sawyer::CommandLine::ParserResult pr;               /**< Wrapped ParserResult. */

    /** Number of times a switch appeared. */
    size_t count(const std::string &swName) const;

    /** Saved values for a switch. */
    parsed_values operator[](const std::string &swName) const;
};

/** Transfer parser results to map. */
void store(const Sawyer::CommandLine::ParserResult &results, variables_map &output);

/** Transfer map to C++ variables.
 *
 *  This wrapper doesn't try to handle a "push" paradigm, but if you've gone around the wrapper to declare switches using
 *  Sawyer's interface and those switches were specified with storage locations, the data has already been moved before this
 *  function is called (it's part of command_line_parser::run). */
void notify(variables_map&);

} // namespace

} // namespace
} // namespace

#endif
