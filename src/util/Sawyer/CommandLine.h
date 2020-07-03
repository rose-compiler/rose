// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




/* Command-line switch parsing. */
#ifndef Sawyer_CommandLine_H
#define Sawyer_CommandLine_H

#include <Sawyer/Assert.h>
#include <Sawyer/DocumentMarkup.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/Set.h>
#include <Sawyer/SharedPointer.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/any.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cerrno>
#include <ctype.h>
#include <list>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace Sawyer { // documented in Sawyer.h

/** Parses program command line switches and arguments.
 *
 * @section sawyer_commandline_defns Definitions
 *
 *  @li A <em>program command line</em> is the vector of strings passed to a program by the operating system or runtime.
 *  @li A <em>commmand line argument</em> is one element of the program command line vector.
 *  @li A <em>switch</em> is a named command line argument, usually introduced with a special character sequence followed
 *      by a name, such as <code>-\-color</code>. The "--" is the <em>switch prefix</em>, "color" is the <em>switch name</em>,
 *      and "--color" is the <em>switch string</em>.
 *  @li A <em>switch argument</em> is an optional value specified on the program command line and associated with a switch,
 *      such as the word "grey" in <code>-\-color=grey</code> or <code>-\-color grey</code> (as two command-line arguments).
 *  @li A <em>switch value</em> is a switch argument that has been converted to a value within a program, such as the
 *      enumeration constant <code>GRAY</code>, whether the string on the command line was "grey" or "gray".
 *  @li A <em>non-switch</em> is a program argument that doesn't appear to be a switch. Another name for the same thing
 *      is <em>positional program argument</em>.
 *
 * @section sawyer_commandline_parts The major parts of the API
 *
 *  Program command-line parsing consists of the following major components:
 *
 *  @li The Switch class declares a switch and specifies such things as the switch name and its arguments.
 *  @li The SwitchGroup class groups related switches into collections.
 *  @li The Parser class parses a program command line to produce a ParserResult.
 *  @li The ParserResult class holds information about how a program command line was parsed by storing, among other things,
 *      a list of ParsedValue objects.
 *  @li The ParsedValue class holds details about each value parsed from a program command line, including the value's
 *      string from the command line and information about the associated switch.
 *
 * @section sawyer_commandline_desc Description
 *
 *  The library is used in three phases: first, the command line is described in terms of switches and their arguments; then a
 *  parser is constructed and applied to the command line to obtain a result; and finally, the result is used by querying or
 *  pushing results into variables.
 *
 *  Some of our goals in designing this library were influenced by other libraries. We wanted to take the good ideas of others
 *  but avoid the same pitfalls.  Our goals are:
 *
 *  @li Keep the switch declaration API as simple and terse as reasonably possible without the loss of self-documenting
 *      code.  Our approach is to use small, optional property-setting functions that can be chained together rather than
 *      excessive function overloading or magical strings.
 *  @li The switch declaration API should be able to describe all the situations that routinely occur in command-line parsing
 *      including things like optional arguments and multiple arguments, various ways to handle multiple occurrences of the
 *      same switch, short and long switch names, various switch prefixes and value separators, nestling of short switches and
 *      their arguments, standard switch actions, etc.  We accomplish this by using a consistent API of switch properties.
 *  @li Parsing of the command line should not only recognize the syntax of the command line, but also parse switch arguments
 *      into a form that is easy to use within the program.  E.g., a string like "1234" should become an <code>int</code> in
 *      the program.
 *  @li Provide an extension mechanism that is easy for beginning programmers to grasp. One way we accomplish this is by
 *      avoiding excessive use of templates for generic programming and instead rely on polymorphic classes and smart
 *      pointers, both of which are common in other object oriented languages.  Parsing command-line arguments is not typically
 *      a performance bottleneck, and when we were faced with the alternatives of performance versus features we chose
 *      features.
 *  @li The library should provide an API that is suitable for both library writers and application programmers.  In other
 *      words, a library that expects to be configured from the command line should provide a description of its command-line
 *      in a way that allows the application to augment it with its own switches, even to the extent that the application can
 *      rename library switches if it desires.
 *  @li The parser results should have a rich query API since this is the point at which it primarily interfaces with the
 *      program.  A program should not have to work hard to use the parsing results. The library provides both pull and
 *      push capabilities for the results: the user can query results or the library can write them directly into user-supplied
 *      variables.
 *  @li Documentation should appear next to the thing it documents, and it should not be necessary to have more than one
 *      source-level copy of documentation.  The library is able to generate complete Unix manual pages as plain text or
 *      [POD](https://en.wikipedia.org/wiki/Plain_Old_Documentation), the latter which can be converted to to a variety of
 *      other formats with standard tools.
 *
 * @section sawyer_commandline_ex1 An example
 *
 *  Here's an example to give the basic flavor of the library.  The description of the command-line and the parsing of the
 *  command-line are combined in a single function, <code>parseCommandLine</code>:
 *
 * @snippet commandLineEx1.C parseCommandLine decl
 *
 *  We'll divide the command-line into two groups of switches: switches that are common to all our tools, and switches that are
 *  specific to this particular tool.  In real life, the common switches would be returned from some other function defined in
 *  a library that all the tools share.  Each switch group may have its own documentation, which causes its switches to be
 *  grouped together in the manual page.  Documentation is free-flowing and automatically wrapped to the width of the terminal
 *  window in which the command runs, so there is no need to have line feeds within the source code string literals, although
 *  they are allowed.  A double line feed in the documentation string is a paragraph separator.
 *
 * @snippet commandLineEx1.C parseCommandLine standard
 *
 *  Now we can declare a couple switches inside our <code>standard</code> switch group.  We'll declare one that will show the
 *  manual page, and another that shows the version number.  The man page is shown with <code>--help</code>, <code>-h</code>,
 *  or <code>-?</code>, and the version is shown with <code>--version</code> or <code>-V</code>.  %Switch actions occur only
 *  when switch parsing is all done (i.e., the man page won't be emitted unless the command-line is parsable).
 *
 * @snippet commandLineEx1.C parseCommandLine helpversion
 *
 *  We place all the tool-specific switches in another switch group we call <code>tool</code>.  The <code>--isa</code> switch
 *  will accept an argument that can be anything and is stored in <code>std::string settings.isaName</code>.  If the argument
 *  is the word "list" then the function that eventually parses it will generate a list (this function is part of the tool
 *  source code and is not part of %Sawyer).
 *
 * @snippet commandLineEx1.C parseCommandLine isa
 *
 *  The next two switches are similar except they expect arguments that are non-negative integers.  The argument will be parsed
 *  by %Sawyer, which accepts decimal, octal, and hexadecimal using C syntax, and eventually assigned to the specified
 *  <code>settings</code> data member, both of which are of type <code>unsigned long</code>.  The parser function,
 *  <code>nonNegativeIntegerParser</code>, is overloaded to accept a variety of integer types and a parsing exception will be
 *  thrown if the argument string parses to a value which doesn't fit in the specified variable.
 *
 * @snippet commandLineEx1.C parseCommandLine at
 *
 *  Now that the command-line switches are described, we can create a parser.  The parser also controls the non-switch aspects
 *  of the manual page, such as the heading and footer, purpose, synopsis, and other sections.
 *
 * @snippet commandLineEx1.C parseCommandLine parser
 *
 *  Finally we can do the actual parsing.  We could split this statement into multiple statements, but one of %Sawyer's idioms
 *  is to chain things together in a functional programming style.  The <code>with</code> method inserts our switch
 *  declarations from above into the parser; the <code>parse</code> method, of which there are multiple overloaded varieties,
 *  does the real work; and the <code>apply</code> method applies the results of a successful parse by copying values into the
 *  variabiables we specified, and running switch actions (e.g., "help" and "version") if necessary.
 *
 * @snippet commandLineEx1.C parseCommandLine parse
 *
 *  Here's the whole example in one shot (the docs/examples/commandLineEx1.C file):
 *
 * @includelineno commandLineEx1.C
 *
 *  And here's the output from the <code>--help</code> switch:
 *
 * @verbinclude commandLineEx1.out
 */
namespace CommandLine {

SAWYER_EXPORT extern const std::string STR_NONE;
class Switch;
class SwitchGroup;
class Parser;
class ParserResult;

/** The order in which things are sorted in the documentation. */
enum SortOrder {
    INSERTION_ORDER,                                    /**< Entities appear in the documentation in the same order they
                                                         *   are inserted into the container.  For instance, manual page
                                                         *   sections will appear in the order of the Parser::doc calls, or
                                                         *   switches are sorted within a switch group according to the order
                                                         *   they were inserted into the group. */
    DOCKEY_ORDER                                        /**< Entities are sorted according to their documentation keys.
                                                         *   Documentation keys, which default to lower-case entity names, are
                                                         *   used to sort the entities within their container. This is the
                                                         *   default. */
};

/** Format of a switch string. */
enum Canonical {
    CANONICAL,                                          /**< Switch strings that are qualified with the switch group name
                                                         *   or which belong to a group that has no name. */
    NONCANONICAL,                                       /**< Switch strings that are not @ref CANONICAL. */
    ALL_STRINGS                                         /**< The union of @ref CANONICAL and @ref NONCANONICAL. */
};

/** How to show group names in switch synopsis. */
enum ShowGroupName {
    SHOW_GROUP_OPTIONAL,                                /**< Show name as being optional, like "--[group-]switch". */
    SHOW_GROUP_REQUIRED,                                /**< Show name as being required, like "--group-switch". */
    SHOW_GROUP_NONE,                                    /**< Never show the group name. */
    SHOW_GROUP_INHERIT                                  /**< Group inherits value from the parser. */
};

/** Whether to skip a switch. */
enum SwitchSkipping {
    SKIP_NEVER,                                         /**< Treat the switch normally. */
    SKIP_WEAK,                                          /**< Process switch normally, but also add to skipped list. */
    SKIP_STRONG                                         /**< Skip switch and its argument(s) without saving any value. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Program argument cursor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Position within a command-line. A command line consists of an ordered set of strings of various lengths, and this object is
 *  an index to a particular character of a particular string.
 *
 * @sa Cursor::location */
struct SAWYER_EXPORT Location {
    size_t idx;                                                 /**< Index into some vector of program argument strings. */
    size_t offset;                                              /**< Character offset within a program argument string. */

    /** Constructs the location of the first character of the first string.  For empty command-lines, this is also the end
     *  location. */
    Location(): idx(0), offset(0) {}

    /** Constructs a location that points to a particular character of a particular string. */
    Location(size_t idx, size_t offset): idx(idx), offset(offset) {}

    /** Equality. Returns true only when this location is equal to @p other. Two locations are equal only when their @ref idx
     *  and @ref offset members are equal. */
    bool operator==(const Location &other) const { return idx==other.idx && offset==other.offset; }

    /** Inequality. Returns true only when this location is not equal to @p other.  Two locations are not equal if either
     *  their @ref idx or @ref offset members are not equal. */
    bool operator!=(const Location &other) const { return !(*this==other); }

    /** Less than.  Returns true only when this location is less than @p other.  If both locations are referring to the same
     * command-line, then this method returns true if this location points to an earlier character than @p other. */
    bool operator<(const Location &other) const { return idx<other.idx || (idx==other.idx && offset<other.offset); }

    /** Less than or equal.  Returns true only when this location is less than or equal to @p other as determined by the
     *  <code><</code> or <code>==</code> operators. */
    bool operator<=(const Location &other) const { return *this<other || *this==other; }
};

/** Print a location. Prints a location as the dotted pair <em>idx</em>.<em>offset</em>. */
SAWYER_EXPORT std::ostream& operator<<(std::ostream&, const Location&);

/** Indicates an invalid location.  The library uses this to indicate that a string came from somewhere other than the
 *  command-line.  The constant <code>NOWHERE</code> compares equal to itself but unequal to (less than) all valid
 *  locations. */
SAWYER_EXPORT extern const Location NOWHERE;

/** Input stream for command line arguments.
 *
 *  A cursor is an ordered set of strings and a current position in that set. */
class SAWYER_EXPORT Cursor {
#include <Sawyer/WarningsOff.h>
    std::vector<std::string> strings_;
    Location loc_;
#include <Sawyer/WarningsRestore.h>
public:
    /** Construct a cursor from an ordered set of strings.  The cursor's initial position is the first character of the first
     *  string, or the end if the set contains no strings or contains only empty strings. */
    Cursor(const std::vector<std::string> &strings): strings_(strings) { location(Location()); }

    /** Constructs a cursor for a single string.  The cursor's initial position is the first character of the string, or the
     *  end if the string is empty. */
    Cursor(const std::string &string): strings_(1, string) { location(Location()); }

    /** Constructs a not-very-useful cursor to nothing. */
    Cursor() {}

    /** All strings for the cursor. */
    const std::vector<std::string>& strings() const { return strings_; }

    /** Property: current position of the cursor.
     *
     *  The following semantics apply to the location:
     *
     *  @li The location's @c idx member will either point to a valid string in the cursor's @ref strings vector, or it will be
     *      equal to the size of that vector.
     *  @li When the @c idx member is equal to the size of the cursor's @ref strings vector, the cursor is said to be at the
     *      end of its input and @ref atEnd returns true.
     *  @li The location's @c offset member is zero whenever @ref atEnd returns true.
     *  @li When @ref atEnd returns false, the @c offset is less than or equal to the length of the string indexed by @c idx.
     *  @li When @ref atEnd returns false and @c offset is equal to the string length, then the cursor is said to be
     *      positioned at the end of an argument and @ref atArgEnd returns true.
     *
     *  When a new location is provided it will be immediately adjusted so that the @c idx member is not greater than the
     *  number of strings in the cursor, and its @c offset member is less than or equal to the length of that string (or zero
     *  when @c idx is equal to the number of strings).
     * @{ */
    const Location& location() const { return loc_; }
    Cursor& location(const Location &loc);
    /** @} */

    /** True when the cursor is at the beginning of an argument. Returns true if the cursor points to an argument and is at the
     * beginning of that argument. Returns false otherwise, including when @ref atEnd returns true. */
    bool atArgBegin() const { return loc_.idx<strings_.size() && 0==loc_.offset; }

    /** True when the cursor is at the end of an argument.  Returns true if the cursor points to an argument and is positioned
     *  past the end of that string.  Returns false otherwise, including when @ref atEnd returns true. */
    bool atArgEnd() const { return loc_.idx<strings_.size() && loc_.offset>=strings_[loc_.idx].size(); }

    /** Returns true when the cursor is after all arguments. When the cursor is after all arguments, then @ref atArgBegin and
     *  @ref atArgEnd both return false. A @p locaton can be specified to override the location that's inherent to this cursor
     *  without changing this cursor.
     * @{ */
    bool atEnd() const { return atEnd(loc_); }
    bool atEnd(const Location &location) const { return location.idx >= strings_.size(); }
    /** @} */

    /** Return the entire current program argument regardless of where the cursor is in that argument.  A @p location can be
     *  specified to override the location that's inherent to this cursor without changing this cursor.  It is an error to call
     *  this when @ref atEnd returns true.
     * @{ */
    const std::string& arg() const { return arg(loc_); }
    const std::string& arg(const Location &location) const;
    /** @} */

    /** Return the part of an argument at and beyond the cursor location.  If the cursor is positioned at the end of an
     *  argument then an empty string is returned.  A @p location can be specified to override the location that's inherent to
     *  this cursor without changing this cursor.  Returns an empty string if called when @ref atEnd returns true.
     * @{ */
    std::string rest() const { return rest(loc_); }
    std::string rest(const Location &location) const;
    /** @} */

    /** Returns all characters within limits.  Returns all the characters between this cursor's current location and the
     *  specified location, which may be left or right of the cursor's location.  The two argument version uses the two
     *  specified locations rather than this cursor's current location.  The @p separator string is inserted between text that
     *  comes from two different strings. @sa linearDistance
     * @{ */
    std::string substr(const Location &limit, const std::string &separator=" ") const { return substr(loc_, limit, separator); }
    std::string substr(const Location &limit1, const Location &limit2, const std::string &separator=" ") const;
    /** @} */

    /** Replace the current string with new strings.  Repositions the cursor to the beginning of the first inserted
     * string. Must not be called when @ref atEnd returns true. */
    void replace(const std::vector<std::string>&);

    /** Advance over characters.  Advances the cursor's current location by @p nchars characters.  The cursor must be
     *  positioned so that at least @p nchars characters can be skipped in the current string.  This call does not advance the
     *  cursor to the next string. */
    void consumeChars(size_t nchars);

    /** Advance the cursor to the beginning of the next string.  If the cursor is already positioned at the last string then it
     * will be positioned to the end and @ref atEnd will return true.  It is permissible to call this method when @ref atEnd
     * already returns true, in which case nothing happens.
     * @{ */
    void consumeArgs(size_t nargs) {
        loc_.idx = std::min(strings_.size(), loc_.idx+nargs);
        loc_.offset = 0;
    }
    void consumeArg() { consumeArgs(1); }
    /** @} */

    /** Number of characters from the beginning of the cursor to its current location.  This is the same as calling
     * @code
     *  substr(Location(), "").size()
     * @endcode
     *  but faster. */
    size_t linearDistance() const;
};

/** Guards a cursor and restores it when the guard is destroyed.  If the guard is destroyed without first calling its @ref
 *  cancel method then the associated cursor's location is reset to its location at the time this guard was constructed. */
class ExcursionGuard {
    Cursor &cursor_;
    Location loc_;
    bool canceled_;
public:
    /** Construct a guard for a cursor.  The guard remembers the cursor's location and restores the location if the
     *  guard is destroyed before its @ref cancel method is called. */
    ExcursionGuard(Cursor &cursor): cursor_(cursor), loc_(cursor.location()), canceled_(false) {} // implicit
    ~ExcursionGuard() { if (!canceled_) cursor_.location(loc_); }

    /** Cancel the excursion guard.  The associated cursor will not be reset to its initial location when this guard is
     *  destroyed. */
    void cancel() { canceled_ = true; }

    /** Starting location.  This is the location to which the cursor is restored when the guard is destroyed, unless @ref
     *  cancel has been called. */
    const Location &startingLocation() const { return loc_; }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch value savers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// used internally
class SAWYER_EXPORT ValueSaver: public SharedObject {
protected:
    ValueSaver() {}
public:
    typedef SharedPointer<ValueSaver> Ptr;
    virtual ~ValueSaver() {}
    virtual void save(const boost::any&, const std::string &switchKey) = 0;
};

// used internally
template<typename T>
class TypedSaver: public ValueSaver {
    T &storage_;
protected:
    TypedSaver(T &storage): storage_(storage) {}
public:
    typedef SharedPointer<TypedSaver> Ptr;
    static Ptr instance(T &storage) { return Ptr(new TypedSaver(storage)); }
    virtual void save(const boost::any &value, const std::string &/*switchKey*/) /*override*/ {
        storage_ = boost::any_cast<T>(value);
    }
};

// Partial specialization of TypedSaver, saving a value of any type T into a container of type CONTAINER_TEMPLATE calling the
// containers INSERT_METHOD with one argument: the value.  The CONTAINER_TEMPLATE is the name of a class template that
// takes one argument: type type of value stored by the container.
#define SAWYER_COMMANDLINE_SEQUENCE_SAVER(CONTAINER_TEMPLATE, INSERT_METHOD)                                                   \
    template<typename T>                                                                                                       \
    class TypedSaver<CONTAINER_TEMPLATE<T> >: public ValueSaver {                                                              \
        CONTAINER_TEMPLATE<T> &storage_;                                                                                       \
    protected:                                                                                                                 \
        TypedSaver(CONTAINER_TEMPLATE<T> &storage): storage_(storage) {}                                                       \
    public:                                                                                                                    \
        static Ptr instance(CONTAINER_TEMPLATE<T> &storage) { return Ptr(new TypedSaver(storage)); }                           \
        virtual void save(const boost::any &value, const std::string &/*switchKey*/) /*override*/ {                            \
            T typed = boost::any_cast<T>(value);                                                                               \
            storage_.INSERT_METHOD(typed);                                                                                     \
        }                                                                                                                      \
    }

// Partial specialization of TypedSaver for saving values into map-like containers. The CONTAINER_TEMPLATE should take two
// parameters: the key type (always std::string) and the value type (not part of this specialization). The value is stored by
// invoking the INSERT_METHOD with two arguments: the key string for the switch whose value is being saved, and the value to
// save.
#define SAWYER_COMMANDLINE_MAP_SAVER(CONTAINER_TEMPLATE, INSERT_METHOD)                                                        \
    template<typename T>                                                                                                       \
    class TypedSaver<CONTAINER_TEMPLATE<std::string, T> >: public ValueSaver {                                                 \
        CONTAINER_TEMPLATE<std::string, T> &storage_;                                                                          \
    protected:                                                                                                                 \
        TypedSaver(CONTAINER_TEMPLATE<std::string, T> &storage): storage_(storage) {}                                          \
    public:                                                                                                                    \
        static Ptr instance(CONTAINER_TEMPLATE<std::string, T> &storage) { return Ptr(new TypedSaver(storage)); }              \
        virtual void save(const boost::any &value, const std::string &switchKey) /*override*/ {                                \
            T typed = boost::any_cast<T>(value);                                                                               \
            storage_.INSERT_METHOD(switchKey, typed);                                                                          \
        }                                                                                                                      \
    }

// Partial specialization of TypedSaver for saving values into map-like containers using the STL approach where the insert
// operator takes an std::pair(key,value) rather than two arguments. The CONTAINER_TEMPLATE should take two parameters: the key
// type (always std::string) and the value type (not part of this specialization). The value is stored by invoking the
// INSERT_METHOD with two arguments: the key string for the switch whose value is being saved, and the value to save.
#define SAWYER_COMMANDLINE_MAP_PAIR_SAVER(CONTAINER_TEMPLATE, INSERT_METHOD)                                                   \
    template<typename T>                                                                                                       \
    class TypedSaver<CONTAINER_TEMPLATE<std::string, T> >: public ValueSaver {                                                 \
        CONTAINER_TEMPLATE<std::string, T> &storage_;                                                                          \
    protected:                                                                                                                 \
        TypedSaver(CONTAINER_TEMPLATE<std::string, T> &storage): storage_(storage) {}                                          \
    public:                                                                                                                    \
        static Ptr instance(CONTAINER_TEMPLATE<std::string, T> &storage) { return Ptr(new TypedSaver(storage)); }              \
        virtual void save(const boost::any &value, const std::string &switchKey) /*override*/ {                                \
            T typed = boost::any_cast<T>(value);                                                                               \
            storage_.INSERT_METHOD(std::make_pair(switchKey, typed));                                                          \
        }                                                                                                                      \
    }

SAWYER_COMMANDLINE_SEQUENCE_SAVER(std::vector, push_back);
SAWYER_COMMANDLINE_SEQUENCE_SAVER(std::list, push_back);
SAWYER_COMMANDLINE_SEQUENCE_SAVER(std::set, insert);
SAWYER_COMMANDLINE_SEQUENCE_SAVER(Sawyer::Container::Set, insert);
SAWYER_COMMANDLINE_SEQUENCE_SAVER(Optional, operator=);
SAWYER_COMMANDLINE_MAP_PAIR_SAVER(std::map, insert);
SAWYER_COMMANDLINE_MAP_SAVER(Sawyer::Container::Map, insert);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parsed value
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about a parsed switch value.
 *
 *  Each time a switch argument is parsed to create a value, whether it comes from the program command line or a default value
 *  string, a ParsedValue object is constructed to describe it.  These objects hold the value, the string from whence the value
 *  was parsed, and information about where the value came from and with which switch it is associated.  This class also
 *  provides a number of methods for conveniently and safely casting the value to other types. */
class SAWYER_EXPORT ParsedValue {
#include <Sawyer/WarningsOff.h>
    boost::any value_;
    Location valueLocation_;                            /**< Where this value came from on the command-line; or NOWHERE. */
    std::string valueString_;                           /**< String representation of the value. */
    std::string switchKey_;                             /**< Key for the switch that parsed this value. */
    Location switchLocation_;                           /**< Start of the switch name in @c switchString_. */
    std::string switchString_;                          /**< Prefix and switch name. */
    size_t keySequence_;                                /**< Relation of this value w.r.t. other values for same key. */
    size_t switchSequence_;                             /**< Relation of this value w.r.t. other values for the switch name. */
    ValueSaver::Ptr valueSaver_;                        /**< Saves the value during ParserResult::apply. */
#include <Sawyer/WarningsRestore.h>

public:
    /** Construct a new empty value.  The @ref isEmpty method will return true for values that are default constructed. */
    ParsedValue(): keySequence_(0), switchSequence_(0) {}

    /** Construct a new value. The type of the value is erased via <code>boost::any</code> so that templates do not need to be
     *  used at the API level.  It is the responsibility of the user to remember the type of the value, although restoration of
     *  the type information via <code>boost::any_cast</code> will check consistency.  The @p loc is the starting location of
     *  the value on the command line, or the constant @ref NOWHERE.  The @p str is the string that was parsed to create the
     *  value (or at least a string representation of the value).  The @p saver is an optional pointer to the functor that's
     *  responsible for pushing the value to a user-specified variable when ParserResult::apply is called.
     *
     *  The arguments specified here are the values returned by @ref value, @ref valueLocation, @ref string, and @ref
     *  valueSaver. */
    ParsedValue(const boost::any value, const Location &loc, const std::string &str, const ValueSaver::Ptr &saver)
        : value_(value), valueLocation_(loc), valueString_(str), keySequence_(0), switchSequence_(0), valueSaver_(saver) {}

    /** Update switch information.  This updates information about the switch that parsed the value. The arguments specified
     *  here will be the values returned by  @ref switchKey, @ref switchLocation, and @ref switchString. */
    ParsedValue& switchInfo(const std::string &key, const Location &loc, const std::string &str) {
        switchKey_ = key;
        switchLocation_ = loc;
        switchString_ = str;
        return *this;
    }

private:
    friend class ParserResult;
    void sequenceInfo(size_t keySequence, size_t switchSequence) {
        keySequence_ = keySequence;
        switchSequence_ = switchSequence;
    }

public:
    /** Property: the parsed value.
     *  Parsed values are represented by <code>boost::any</code>, which is capable of storing any type of
     *  parsed value including void.  This is the most basic access to the value; the class also provides a variety of casting
     *  accessors that are sometimes more convenient (their names start with the word "as").
     * @{ */
    const boost::any& value() const { return value_; }
    void value(const boost::any &v) { value_ = v; }
    /** @} */

    /** Property: command-line location from whence this value came.
     *  For values that are defaults which didn't come from the command-line, the constant @ref NOWHERE is returned.
     * @{ */
    Location valueLocation() const { return valueLocation_; }
    ParsedValue& valueLocation(const Location &loc) { valueLocation_ = loc; return *this; }
    /** @} */

    /** String representation.  This is the string that was parsed to create the value. */
    const std::string &string() const { return valueString_; }

    /** Convenience cast.  This returns the value cast to the specified type.  Whereas <code>boost::any_cast</code> requires an
     *  exact type match for the cast to be successful, this method makes more of an effort to be successful.  It recognizes a
     *  variety of common types; less common types will need to be explicitly converted by hand.  In any case, the original
     *  string representation and parser are available if needed.
     * @{ */
    int asInt() const;
    unsigned asUnsigned() const;
    long asLong() const;
    unsigned long asUnsignedLong() const;
    boost::int64_t asInt64() const;
    boost::uint64_t asUnsigned64() const;
    double asDouble() const;
    float asFloat() const;
    bool asBool() const;
    std::string asString() const;
    /** @} */

    /** Convenient any_cast.  This is a slightly less verbose way to get the value and perform a
     * <code>boost::any_cast</code>. */
    template<typename T> T as() const { return boost::any_cast<T>(value_); }

    /** Property: switch key. The key used by the switch that created this value.
     * @{ */
    ParsedValue& switchKey(const std::string &s) { switchKey_ = s; return *this; }
    const std::string& switchKey() const { return switchKey_; }
    /** @} */

    /** The string for the switch that caused this value to be parsed.  This string includes the switch prefix and the switch
     *  name in order to allow programs to distinguish between the same switch occuring with two different prefixes (like the
     *  "-invert" vs "+invert" style which is sometimes used for Boolean-valued switches).
     *
     *  For nestled short switches, the string returned by this method doesn't necessarily appear anywhere on the program
     *  command line.  For instance, this method might return "-b" when the command line was "-ab", because "-a" is a different
     *  switch with presumably a different set of parsed values. */
    const std::string& switchString() const { return switchString_; }

    /** The command-line location of the switch to which this value belongs.  The return value indicates the start of the
     *  switch name after any leading prefix.  For nestled single-character switches, the location's command line argument
     *  index will be truthful, but the character offset will refer to the string returned by @ref switchString. */
    Location switchLocation() const { return switchLocation_; }

    /** How this value relates to others with the same key.  This method returns the sequence number for this value among all
     *  values created for the same switch key. */
    size_t keySequence() const { return keySequence_; }

    /** How this value relates to others created by the same switch.  This method returns the sequence number for this value
     *  among all values created for switches with the same Switch::preferredName. */
    size_t switchSequence() const { return switchSequence_; }

    /** How to save a value at a user-supplied location.  These functors are used internally by the library and users don't
     * usually see them. */
    const ValueSaver::Ptr valueSaver() const { return valueSaver_; }

    /** Save this value in switch-supplied storage. This calls the functor returned by @ref valueSaver in order to save this
     *  parsed value to a user-specified variable in a type-specific manner. */
    void save() const;

    /** True if the value is void. Returns true if this object has no value. */
    bool isEmpty() const { return value_.empty(); }

    /** Print some debugging information. */
    void print(std::ostream&) const;
};

/** Print some information about a parsed value. */
SAWYER_EXPORT std::ostream& operator<<(std::ostream&, const ParsedValue&);

/** A vector of parsed values. */
typedef std::vector<ParsedValue> ParsedValues;




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch argument parsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class parsing a value from input.
 *
 *  A ValueParser is a functor that attempts to recognize the next few characters of a command-line and to convert those
 *  characters to a value of some type. These are two separate but closely related operations.  Subclasses of ValueParser must
 *  implement one of the <code>operator()</code> methods: either the one that takes a Cursor reference argument, or the one
 *  that takes pointers to C strings a la <code>strtod</code>.  The cursor-based approach allows a match to span across several
 *  program arguments, and the C string pointer approach is a very simple interface.
 *
 *  If the parser is able to recognize the next characters of the input then it indicates so by advancing the cursor or
 *  updating the @p endptr argument; if no match occurs then the cursor is not changed and the @p endptr should point to the
 *  beginning of the string, and the parser throws an <code>std::runtime_error</code>.  If, after recognizing and consuming
 *  some input, the parser is able to convert the recognized input into a value, then the value is returned as a ParsedValue,
 *  which includes information about where the value came from, otherwise an <code>std::runtime_error</code> is thrown. When an
 *  error is thrown, the caller can distinguish between the two failure modes by examining whether characters were consumed.
 *
 *  For instance, a parser that recognizes non-negative decimal integers would match (consume) any positive number of
 *  consecutive digits. However, if that string represents a mathematical value which is too large to store in the return type,
 *  then the parser should throw an exception whose string describes the problem, like "integer overflow", or "argument is too
 *  large to store in an 'int'".
 *
 *  Value parsers are always allocated on the heap and reference counted.  Each value parser defines a class factory method,
 *  <code>instance</code>, to allocate a new object and return a pointer to it. The pointer types are named <code>Ptr</code>
 *  and are defined within the class.  For convenience, the parsers built into the library also have global factory functions
 *  which have the same name as the class but start with an initial lower-case letter (see @ref sawyer_parser_factories for
 *  details).  For instance:
 *
 * @code
 *  class IntegerParser;                                // recognizes integers
 *  IntegerParser::Ptr ptr = IntegerParser::instance(); // construct a new object
 *  IntegerParser::Ptr ptr = integerParser();           // another constructor
 * @endcode
 *
 *  Most of the library-provided functors are template classes whose argument specifies the type to use when creating a
 *  ParsedValue object. The constructors take an optional L-value in which to store the parsed value when the
 *  ParserResult::apply method is called.  The factory functions are also templates with an optional argument, but template
 *  argument can be inferred by the compiler.  As an example, here are three ways to call the factory function for the
 *  IntegerParser classes:
 *
 * @code
 *  IntegerParser::Ptr ip1 = integerParser();           // stores value in ParserResult as int
 *  IntegerParser::Ptr ip2 = integerParser<short>();    // stores value in ParserResult as short
 *  long x;
 *  IntegerParser::Ptr ip3 = integerParser(x);          // stores value in ParserResult and x as long
 * @endcode
 *
 *  The values are stored in a ParserResult object during the Parser::parse call, but are not moved into user-specified
 *  L-values until ParserResult::apply is called.
 *
 *  Users can create their own parsers, and are encouraged to do so, by following this same recipe.
 *
 * @sa
 *  @ref sawyer_parser_factories */
class SAWYER_EXPORT ValueParser: public SharedObject, public SharedFromThis<ValueParser> {
#include <Sawyer/WarningsOff.h>
    ValueSaver::Ptr valueSaver_;
#include <Sawyer/WarningsRestore.h>
protected:
    /** Constructor for derived classes. Non-subclass users should use @c instance or factories instead. */
    ValueParser() {}

    /** Constructor for derived classes. Non-subclass users should use @c instance or factories instead. */
    explicit ValueParser(const ValueSaver::Ptr &valueSaver): valueSaver_(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ValueParser> Ptr;

    virtual ~ValueParser() {}

    /** Parse the entire string and return a value.  The matching of the parser against the input is performed by calling
     *  @ref match, which may throw an exception if the input is matched but cannot be converted to a value (e.g., integer
     *  overflow).  If the parser did not match the entire string, then an <code>std::runtime_error</code> is thrown. */
    ParsedValue matchString(const std::string&) /*final*/;

    /** Parse a value from the beginning of the specified string.  If the parser does not recognize the input then it throws an
     *  <code>std::runtime_error</code> without updating the cursor.  If the parser recognizes the input but cannot convert it
     *  to a value (e.g., integer overflow) then the cursor should be updated to show the matching region before the matching
     *  operator throws the <code>std::runtime_error</code> exception. */
    ParsedValue match(Cursor&) /*final*/;

    /** Property: functor responsible for saving a parsed value in user storage.  Many of the ValueParser subclasses take an
     *  argument which is a reference to a user-defined storage location, such as:
     *
     * @code
     *  bool verbose;
     *  Switch("verbose")
     *      .intrinsicValue("true", booleanParser(verbose));
     * @endcode
     *
     *  When a parser is created in such a way, a ValueSaver object is created and recorded in this property. After
     *  parsing, if the user invokes ParserResult::apply, the parsed value is saved into the user location.  No value
     *  is saved until apply is called--this allows command-lines to be parsed for their error side effects without actually
     *  changing any program state.
     * @{ */
    Ptr valueSaver(const ValueSaver::Ptr &f) { valueSaver_ = f; return sharedFromThis(); }
    const ValueSaver::Ptr valueSaver() const { return valueSaver_; }
    /** @} */

private:
    /** Parse a string and return its value. See ValueParser class documentation and @ref match for semantics.
     * @{ */
    virtual ParsedValue operator()(Cursor&);
    virtual ParsedValue operator()(const char *input, const char **endptr, const Location&);
    /** @} */
};

// used internally to convert from one type to another via boost::lexical_cast or throw a runtime_error with a decent message.
template<typename T>
struct LexicalCast {
    static T convert(const std::string &src) {
        try {
            return boost::lexical_cast<T>(src);
        } catch (const boost::bad_lexical_cast &e) {
            throw std::runtime_error(e.what());
        }
    }
};

template<>
struct LexicalCast<boost::regex> {
    static boost::regex convert(const std::string &src) {
        return boost::regex(src);
    }
};

template<>
struct LexicalCast<boost::any> {
    static boost::any convert(const std::string &src) {
        return src;
    }
};

template<typename T>
struct LexicalCast<Optional<T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<Sawyer::Container::Set<T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<Sawyer::Container::Map<std::string, T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<std::vector<T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<std::set<T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<std::list<T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

template<typename T>
struct LexicalCast<std::map<std::string, T> > {
    static T convert(const std::string &src) {
        return LexicalCast<T>::convert(src);
    }
};

/** Parses any argument as plain text.
 *
 *  This parser consumes the entire following string and then attempts to convert it to type T, or throw an
 *  <code>std::runtime_error</code>. This differs from most other parsers which consume only those characters they recognize.
 *  For instance:
 *
 * @code
 *  int i;
 *  anyParser(i);
 *  integerParser(i);
 * @endcode
 *
 *  The <code>anyParser(i)</code> will throw an error for a command line like <code>-n123x</code>, but the
 *  <code>integerParser(i)</code> will consume only the "123" and leave "x" alone (which is presumably the "-x" nestled short
 *  switch).  The <code>integerParser</code> will also provide more informative error messages for overflows.
 *
 *  This parser is primarily intended for parsing arbitrary strings as <code>std::string</code>, and uses that type when no
 *  other type is provided by template argument or constructor argument.
 *
 *  The <code>boost::lexical_cast</code> package is used for the conversion, but <code>boost::bad_lexical_cast</code>
 *  exceptions are caught and rethrown as <code>std::runtime_error</code> as required by the ValueParser interface.
 *
 * @sa @ref anyParser factory method, and @ref sawyer_parser_factories */
template<typename T>
class AnyParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    AnyParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    AnyParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<AnyParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new AnyParser object.  Uses will most likely want to use the @ref
     *  anyParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new AnyParser); }

    /** Allocating constructor. Returns a pointer to a new AnyParser object.  Uses will most likely want to use the @ref
     *  anyParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new AnyParser(valueSaver)); }
private:
    virtual ParsedValue operator()(Cursor &cursor) /*override*/ {
        if (cursor.atEnd())
            throw std::runtime_error("string expected");
        Location startLoc = cursor.location();
        std::string s = cursor.rest();
        cursor.consumeChars(s.size());
        return ParsedValue(LexicalCast<T>::convert(s), startLoc, s, valueSaver());
    }
};

// used internally to cast one numeric type to another and throw a range_error with a decent message.
template<typename Target, typename Source>
struct NumericCast {
    static Target convert(Source from, const std::string &parsed) {
        try {
            return boost::numeric_cast<Target>(from);
        } catch (const boost::numeric::positive_overflow&) {
            std::string bound = boost::lexical_cast<std::string>(boost::numeric::bounds<Target>::highest());
            throw std::range_error("parsed string \""+parsed+"\" is greater than "+bound);
        } catch (const boost::numeric::negative_overflow&) {
            std::string bound = boost::lexical_cast<std::string>(boost::numeric::bounds<Target>::lowest());
            throw std::range_error("parsed string \""+parsed+"\" is less than "+bound);
        } catch (const boost::numeric::bad_numeric_cast&) {
            throw std::range_error("cannot cast \""+parsed+"\" to destination type");
        }
    }
};

// partial specialization for Sawyer::Optional<Target>
template<typename Target, typename Source>
struct NumericCast<Optional<Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for std::vector<Target>
template<typename Target, typename Source>
struct NumericCast<std::vector<Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for std::list<Target>
template<typename Target, typename Source>
struct NumericCast<std::list<Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for std::set<Target>
template<typename Target, typename Source>
struct NumericCast<std::set<Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for Sawyer::Container::Set<Target>
template<typename Target, typename Source>
struct NumericCast<Sawyer::Container::Set<Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for std::map<std::string, Target>
template<typename Target, typename Source>
struct NumericCast<std::map<std::string, Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

// partial specialization for Sawyer::Container::Map<std::string, Target>
template<typename Target, typename Source>
struct NumericCast<Sawyer::Container::Map<std::string, Target>, Source> {
    static Target convert(Source from, const std::string &parsed) {
        return NumericCast<Target, Source>::convert(from, parsed);
    }
};

/** Parses an integer and converts it to numeric type @p T.
 *
 *  Matches an integer in the mathematical sense in C++ decimal, octal, or hexadecimal format, and attempts to convert it to
 *  the type @p T.  If the integer cannot be converted to type @p T then an <code>std::range_error</code> is thrown, which is
 *  most likely caught by higher layers of the library and converted to an <code>std::runtime_error</code> with additional
 *  information about the failure.  The syntax is that which is recognized by the @c strtoll function, plus trailing white
 *  space.
 *
 * @sa @ref integerParser factory, and @ref sawyer_parser_factories. */
template<typename T>
class IntegerParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    IntegerParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    explicit IntegerParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<IntegerParser> Ptr;

    /** Allocating constructor.
     *
     *  Returns a pointer to a new IntegerParser object.  Uses will most likely want to use the @ref integerParser factory
     *  instead, which requires less typing.
     *
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new IntegerParser); }

    /** Allocating constructor.
     *
     *  Returns a pointer to a new IntegerParser object.  Uses will most likely want to use the @ref integerParser factory
     *  instead, which takes the same arguments, but requires less typing.
     *
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new IntegerParser(valueSaver)); }
private:
    virtual ParsedValue operator()(const char *input, const char **rest, const Location &loc) /*override*/ {
        errno = 0;
        boost::int64_t big = strtoll(input, (char**)rest, 0);
        if (*rest==input)
            throw std::runtime_error("integer expected");
        while (isspace(**rest)) ++*rest;
        std::string parsed(input, *rest-input);
        if (ERANGE==errno)
            throw std::range_error("integer overflow when parsing \""+parsed+"\"");
        return ParsedValue(NumericCast<T, boost::int64_t>::convert(big, parsed), loc, parsed, valueSaver());
    }
};

/** Parses a non-negative integer and converts it to numeric type @p T.
 *
 *  Matches a non-negative integer in the mathematical sense in C++ decimal, octal, or hexadecimal format, and attempts to
 *  convert it to the type @p T.  If the integer cannot be converted to type @p T then an <code>std::range_error</code> is
 *  thrown, which is most likely caught by higher layers of the library and converted to an <code>std::runtime_error</code>
 *  with additional information about the failure.  The syntax is that which is recognized by the @c strtoull function except
 *  that a leading minus sign is not allowed (yes, strtoull parses negative numbers and returns them as unsigned), plus
 *  trailing white space.
 *
 * @sa @ref nonNegativeIntegerParser factory, and @ref sawyer_parser_factories. */
template<typename T>
class NonNegativeIntegerParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    NonNegativeIntegerParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    NonNegativeIntegerParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<NonNegativeIntegerParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new NonNegativeIntegerParser object.  Uses will most likely want to use
     *  the @ref nonNegativeIntegerParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new NonNegativeIntegerParser); }

    /** Allocating constructor. Returns a pointer to a new NonNegativeIntegerParser object.  Uses will most likely want to use
     *  the @ref nonNegativeIntegerParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new NonNegativeIntegerParser(valueSaver)); }
private:
    virtual ParsedValue operator()(const char *input, const char **rest, const Location &loc) /*override*/ {
        errno = 0;
        while (isspace(*input)) ++input;
        if ('+'!=*input && !isdigit(*input))
            throw std::runtime_error("unsigned integer expected");
        boost::uint64_t big = strtoull(input, (char**)rest, 0);
        if (*rest==input)
            throw std::runtime_error("unsigned integer expected");
        while (isspace(**rest)) ++*rest;
        std::string parsed(input, *rest-input);
        if (ERANGE==errno)
            throw std::range_error("integer overflow when parsing \""+parsed+"\"");
        return ParsedValue(NumericCast<T, boost::uint64_t>::convert(big, parsed), loc, parsed, valueSaver());
    }
};

/** Parses a positive integer and converts it to numeric type @p T.
 *
 *  Matches a positive integer in the mathematical sense in C++ decimal, octal, or hexadecimal format, and attempts to
 *  convert it to the type @p T.  If the integer cannot be converted to type @p T then an <code>std::range_error</code> is
 *  thrown, which is most likely caught by higher layers of the library and converted to an <code>std::runtime_error</code>
 *  with additional information about the failure.  The syntax is that which is recognized by the @c strtoull function except
 *  that a leading minus sign is not allowed (yes, strtoull parses negative numbers and returns them as unsigned), plus
 *  trailing white space.
 *
 * @sa @ref positiveIntegerParser factory, and @ref sawyer_parser_factories. */
template<typename T>
class PositiveIntegerParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    PositiveIntegerParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    PositiveIntegerParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<PositiveIntegerParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new PositiveIntegerParser object.  Uses will most likely want to use
     *  the @ref positiveIntegerParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new PositiveIntegerParser); }

    /** Allocating constructor. Returns a pointer to a new PositiveIntegerParser object.  Uses will most likely want to use
     *  the @ref positiveIntegerParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new PositiveIntegerParser(valueSaver)); }
private:
    virtual ParsedValue operator()(const char *input, const char **rest, const Location &loc) /*override*/ {
        errno = 0;
        while (isspace(*input)) ++input;
        if ('+'!=*input && !isdigit(*input))
            throw std::runtime_error("positive integer expected");
        boost::uint64_t big = strtoull(input, (char**)rest, 0);
        if (*rest==input || big==0)
            throw std::runtime_error("positive integer expected");
        while (isspace(**rest)) ++*rest;
        std::string parsed(input, *rest-input);
        if (ERANGE==errno)
            throw std::range_error("integer overflow when parsing \""+parsed+"\"");
        return ParsedValue(NumericCast<T, boost::uint64_t>::convert(big, parsed), loc, parsed, valueSaver());
    }
};

/** Parses a real number and converts it to numeric type @p T.
 *
 *  Matches a real number in the mathematical sense in C++ floating-point representation, and attempts to convert it to the
 *  type @p T.  If the real number cannot be converted to type @p T then an <code>std::range_error</code> is thrown, which is
 *  most likely caught by higher layers of the library and converted to an <code>std::runtime_error</code> with additional
 *  information about the failure.  The syntax is that which is recognized by the @c strtod function, plus trailing white
 *  space.
 *
 * @sa @ref realNumberParser factory, and @ref sawyer_parser_factories. */
template<typename T>
class RealNumberParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    RealNumberParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    RealNumberParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<RealNumberParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new RealNumberParser object.  Uses will most likely want to use
     *  the @ref realNumberParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new RealNumberParser); }

    /** Allocating constructor. Returns a pointer to a new RealNumberParser object.  Uses will most likely want to use
     *  the @ref realNumberParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new RealNumberParser(valueSaver)); }
private:
    virtual ParsedValue operator()(const char *input, const char **rest, const Location &loc) /*override*/ {
        double big = strtod(input, (char**)rest);
        if (*rest==input)
            throw std::runtime_error("real number expected");
        while (isspace(**rest)) ++*rest;
        std::string parsed(input, *rest-input);
        return ParsedValue(NumericCast<T, double>::convert(big, parsed), loc, parsed, valueSaver());
    }
};

/** Parses a boolean value and converts it to numeric type @p T.
 *
 *  Matches any one of the strings "1", "t", "true", "y", "yes", "on" (as a true value), "0", "f", "false", "n", "no", and
 *  "off" (as a false value) followed by white space and attempts to convert it to the type @p T.
 *
 * @sa @ref booleanParser factory, and @ref sawyer_parser_factories. */
template<typename T>
class BooleanParser: public ValueParser {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    BooleanParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    BooleanParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<BooleanParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new BooleanParser object.  Uses will most likely want to use the @ref
     *  booleanParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new BooleanParser); }

    /** Allocating constructor. Returns a pointer to a new booleanParser object.  Uses will most likely want to use the @ref
     *  booleanParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new BooleanParser(valueSaver)); }
private:
    virtual ParsedValue operator()(const char *input, const char **rest, const Location &loc) /*override*/ {
        static const char *neg[] = {"false", "off", "no", "0", "f", "n"}; // longest to shortest
        static const char *pos[] = {"true",  "yes", "on", "1", "t", "y"};
        const char *start = input;
        while (isspace(*input)) ++input;
        for (int negpos=0; negpos<2; ++negpos) {
            const char **list = 0==negpos ? neg : pos;
            size_t listsz = 0==negpos ? sizeof(neg)/sizeof(*neg) : sizeof(pos)/sizeof(*pos);
            for (size_t i=0; i<listsz; ++i) {
                if (0==my_strncasecmp(list[i], input, strlen(list[i]))) {
                    *rest = input + strlen(list[i]);
                    while (isspace(**rest)) ++*rest;
                    std::string parsed(start, *rest-start);
                    return ParsedValue(NumericCast<T, bool>::convert(0!=negpos, parsed), loc, parsed, valueSaver());
                }
            }
        }
        throw std::runtime_error("Boolean expected");
    }

    // Microsoft doesn't have the POSIX.1-2001 strncasecmp function
    int my_strncasecmp(const char *a, const char *b, size_t nchars) {
        ASSERT_not_null(a);
        ASSERT_not_null(b);
        for (size_t i=0; i<nchars; ++i) {
            if (!a[i] || !b[i])
                return a[i] ? 1 : (b[i] ? -1 : 0);
            char achar = (char)tolower(a[i]);
            char bchar = (char)tolower(b[i]);
            if (achar != bchar)
                return achar < bchar ? -1 : 1;
        }
        return 0;
    }
};

/** Parses any one of a set of strings.
 *
 *  Recognizes any of the strings register via @ref with method and returns a ParsedValue of type <code>std::string</code> for
 *  the string that was matched.  Longer matches are preferred over shorter matches; if the input contains "bandana" and the
 *  parser knows about "ban", "band", and "bandana", it will match "bandana". The return value is a ParsedValue whose value
 *  is an <code>std::string</code>.
 *
 * @sa sawyer_parser_factories */
class SAWYER_EXPORT StringSetParser: public ValueParser {
#include <Sawyer/WarningsOff.h>
    std::vector<std::string> strings_;
#include <Sawyer/WarningsRestore.h>
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    StringSetParser() {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    StringSetParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<StringSetParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new StringSetParser object.  Uses will most likely want to use
     *  the @ref stringSetParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new StringSetParser); }

    /** Allocating constructor. Returns a pointer to a new StringSetParser object.  Uses will most likely want to use
     *  the @ref stringSetParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new StringSetParser(valueSaver)); }

    /** Adds string members.  Inserts an additional string to be recognized in the input.
     *  @{ */
    Ptr with(const std::string &s) { return with(&s, &s+1); }
    Ptr with(const std::vector<std::string> sv) { return with(sv.begin(), sv.end()); }
    template<class InputIterator>
    Ptr with(InputIterator begin, InputIterator end) {
        strings_.insert(strings_.end(), begin, end);
        return sharedFromThis().dynamicCast<StringSetParser>();
    }
    /** @} */

private:
    virtual ParsedValue operator()(Cursor&) /*override*/;
};

/** Parses an enumerated constant.
 *
 *  Parses one of the strings added via @ref with and returns the associated enumerated constant. The template parameter @p T
 *  is the enum type. Returns  A ParsedValue whose value if type @p T.  This parser uses StringSetParser, thus it prefers to
 *  match enum members with the longest names.
 *
 * @sa sawyer_parser_factories */
template<typename T>
class EnumParser: public ValueParser {
    StringSetParser::Ptr strParser_;
    Container::Map<std::string, T> members_;
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    EnumParser(): strParser_(StringSetParser::instance()) {}

    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    EnumParser(const ValueSaver::Ptr &valueSaver): ValueParser(valueSaver), strParser_(StringSetParser::instance()) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<EnumParser> Ptr;

    /** Allocating constructor. Returns a pointer to a new EnumParser object.  Uses will most likely want to use the @ref
     *  enumParser factory instead, which requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance() { return Ptr(new EnumParser); }

    /** Allocating constructor. Returns a pointer to a new EnumParser object.  Uses will most likely want to use the @ref
     *  enumParser factory instead, which takes the same arguments, but requires less typing.
     * @sa sawyer_parser_factories */
    static Ptr instance(const ValueSaver::Ptr &valueSaver) { return Ptr(new EnumParser(valueSaver)); }

    /** Adds enum members.  Inserts an additional enumeration constant and its string name. */
    Ptr with(const std::string &name, T value) {
        strParser_->with(name);
        members_.insert(name, value);
        return sharedFromThis().template dynamicCast<EnumParser>();
    }
private:
    virtual ParsedValue operator()(Cursor &cursor) /*override*/ {
        ParsedValue strVal = strParser_->match(cursor);
        return ParsedValue(members_[strVal.string()], strVal.valueLocation(), strVal.string(), valueSaver());
    }
};

/** Parses a list of values.
 *
 *  Parses a list of values separated by specified regular expressions.  Each member of the list may have its own parser and
 *  following separator. The final parser and separator are reused as often as necessary. The return value is a ParsedValue
 *  whose value is an STL @c list with members that are the ParsedValue objects return by the list element parsers.
 *
 * @sa sawyer_parser_factories */
class SAWYER_EXPORT ListParser: public ValueParser {
    typedef std::pair<ValueParser::Ptr, std::string> ParserSep;
#include <Sawyer/WarningsOff.h>
    std::vector<ParserSep> elements_;
    size_t minLength_, maxLength_;                      // limits on the number of values permitted
#include <Sawyer/WarningsRestore.h>
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    ListParser(const ValueParser::Ptr &firstElmtType, const std::string &separatorRe)
        : minLength_(1), maxLength_((size_t)-1) {
        elements_.push_back(ParserSep(firstElmtType, separatorRe));
    }
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ListParser> Ptr;

    /** Value type for list ParsedValue. */
    typedef std::list<ParsedValue> ValueList;

    /** Allocating constructor.
     *
     *  The @p firstElmtType is the parser for the first value (and the remaining values also if no subsequent parser is
     *  specified), and the @p separatorRe is the regular expression describing how values of this type are separated from
     *  subsequent values. The default separator is a comma, semicolon, or colon followed by zero or more white space
     *  characters. Users will most likely want to use the @ref listParser factory instead, which takes the same arguments
     *  but requires less typing. */
    static Ptr instance(const ValueParser::Ptr &firstElmtType, const std::string &separatorRe="[,;:]\\s*") {
        return Ptr(new ListParser(firstElmtType, separatorRe));
    }

    /** Specifies element type and separator.
     *
     *  Adds another element type and separator to this parser.  The specified values are also used for all the following list
     *  members unless a subsequent type and separator are supplied.  I.e., the final element type and separator are repeated
     *  as necessary when parsing. The default separator is a comma, semicolon, or colon followed by zero ore more white space
     *  characters. */
    Ptr nextMember(const ValueParser::Ptr &elmtType, const std::string &separatorRe="[,;:]\\s*") {
        elements_.push_back(ParserSep(elmtType, separatorRe));
        return sharedFromThis().dynamicCast<ListParser>();
    }

    /** Specify limits for the number of values parsed.
     *
     *  By default, a list parser parses zero or more values with no limit.  The @ref limit method provides separate lower and
     *  upper bounds (the one argument version sets only the upper bound), and the @ref exactly method is a convenience to set
     *  the lower and upper bound to the same value.
     * @{ */
    Ptr limit(size_t minLength, size_t maxLength);
    Ptr limit(size_t maxLength) { return limit(std::min(minLength_, maxLength), maxLength); }
    Ptr exactly(size_t length) { return limit(length, length); }
    /** @} */
private:
    virtual ParsedValue operator()(Cursor&) /*override*/;
};

/** @defgroup sawyer_parser_factories Command line parser factories
 *  @ingroup sawyer
 *
 *  Factories for creating instances of Sawyer::CommandLine::ValueParser subclasses.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for the various
 *  subclasses of @ref ValueParser, and they sometimes alleviate the user from having to specify template arguments.  They take
 *  the same arguments as the @c instance method(s) and have the same name as the class they return, except their initial
 *  letter is lower case.  These two calls are equivalent:
 *
 * @code
 *  ValueParser::Ptr parser1 = IntegerParser::instance<short>(storage);
 *  ValueParser::Ptr parser2 = integerParser(storage);
 * @endcode
 *
 *  Most parser factories come in three varieties:
 *
 *  @li A factory that takes no function or template arguments creates a parser that uses a specific, hard-coded C++ type
 *      to represent its value and does not attempt to copy the parsed value to a user-specified storage location. The value,
 *      assuming it is kept, is stored only in the ParserResult.
 *  @li A factory that takes a template typename argument creates a parser that uses the specified type to represent its value,
 *      and does not attempt to copy the parsed value to a user-supplied storage location. The value, assuming it is kept, is
 *      stored only in the ParserResult.
 *  @li A factory that takes a function argument that is an L-value (and an inferred template argument) creates a parser that
 *      uses the inferred type to represent its value and also stores that value in the user-supplied location when
 *      ParserResult::apply is called.  If the L-value is an <code>std::vector</code> then parsed values are pushed onto the
 *      back of the the vector.
 *
 *  The @ref integerParser factory is an example for all three of these varieties:
 *
 * @code
 *  integerParser()         // saves an int value in ParserResult
 *  integerParser<short>()  // saves a short value in ParserResult
 *  long result;
 *  integerParser(result);  // saves a long value in ParserResult and in result
 *  std::vector<int> results;
 *  integerParser(results); // saves more than one integer
 * @endcode
 *
 * @section factories Factories
 */


/** @ingroup sawyer_parser_factories
 *  @brief Factory for value parsers.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for ValueParser
 *  subclasses and often alleviates the user from having to specify template arguments.  Most parser factories come in two
 *  varieties, and some in three varieties.
 *
 * @sa
 * @li @ref sawyer_parser_factories
 * @li @ref ValueParser base class
 * @li Documentation for the returned class
 * @{ */
template<typename T>
typename AnyParser<T>::Ptr anyParser(T &storage) {
    return AnyParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename AnyParser<T>::Ptr anyParser() {
    return AnyParser<T>::instance();
}
SAWYER_EXPORT AnyParser<std::string>::Ptr anyParser();

template<typename T>
typename IntegerParser<T>::Ptr integerParser(T &storage) {
    return IntegerParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename IntegerParser<T>::Ptr integerParser() {
    return IntegerParser<T>::instance();
}
SAWYER_EXPORT IntegerParser<int>::Ptr integerParser();

template<typename T>
typename NonNegativeIntegerParser<T>::Ptr nonNegativeIntegerParser(T &storage) {
    return NonNegativeIntegerParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename NonNegativeIntegerParser<T>::Ptr nonNegativeIntegerParser() {
    return NonNegativeIntegerParser<T>::instance();
}
SAWYER_EXPORT NonNegativeIntegerParser<unsigned>::Ptr nonNegativeIntegerParser();

template<typename T>
typename PositiveIntegerParser<T>::Ptr positiveIntegerParser(T &storage) {
    return PositiveIntegerParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename PositiveIntegerParser<T>::Ptr positiveIntegerParser() {
    return PositiveIntegerParser<T>::instance();
}
SAWYER_EXPORT PositiveIntegerParser<unsigned>::Ptr positiveIntegerParser();

template<typename T>
typename RealNumberParser<T>::Ptr realNumberParser(T &storage) {
    return RealNumberParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename RealNumberParser<T>::Ptr realNumberParser() {
    return RealNumberParser<T>::instance();
}
SAWYER_EXPORT RealNumberParser<double>::Ptr realNumberParser();

template<typename T>
typename BooleanParser<T>::Ptr booleanParser(T &storage) {
    return BooleanParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename BooleanParser<T>::Ptr booleanParser() {
    return BooleanParser<T>::instance();
}
SAWYER_EXPORT BooleanParser<bool>::Ptr booleanParser();

template<typename T>
typename EnumParser<T>::Ptr enumParser(T &storage) {
    return EnumParser<T>::instance(TypedSaver<T>::instance(storage));
}
template<typename T>
typename EnumParser<T>::Ptr enumParser(std::vector<T> &storage) {
    return EnumParser<T>::instance(TypedSaver<std::vector<T> >::instance(storage));
}
template<typename T>
typename EnumParser<T>::Ptr enumParser(Optional<T> &storage) {
    return EnumParser<T>::instance(TypedSaver<Optional<T> >::instance(storage));
}
template<typename T>
typename EnumParser<T>::Ptr enumParser() {
    return EnumParser<T>::instance();
}

SAWYER_EXPORT StringSetParser::Ptr stringSetParser(std::string &storage);
SAWYER_EXPORT StringSetParser::Ptr stringSetParser();

SAWYER_EXPORT ListParser::Ptr listParser(const ValueParser::Ptr&, const std::string &sepRe="[,;:]\\s*");
/** @} */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch argument descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes one argument of a command-line switch.
 *
 *  A SwitchArgument declares one argument for a switch. Each argument should have a string name that will appear in
 *  documentation and error messages.  Each argument also has a ValueParser for parsing the argument, and an optional default
 *  value to use if the argument is missing (the presence of a default value makes the argument optional). If a default value
 *  is specified then it must be parsable by the specified parser.
 *
 *  A SwitchArgument declares only one argument. Some switches may have more than one argument, like <code>-\-swap a b</code>,
 *  in which case their declaration would have two SwitchArgument objects.  Some switches take one argument which is a list of
 *  values, like <code>-\-swap a,b</code>, which is described by declaring a single argument with @c listParser.  Some switches
 *  may occur multiple times to specify their arguments, like <code>-\-swap a -\-swap b</code>, in which case the switch is
 *  declared to have one argument and to save a value for each occurrence (see Switch::whichValue).
 *
 *  Users seldom use this class directly, but rather call Switch::argument to declare arguments. */
class SAWYER_EXPORT SwitchArgument {
#include <Sawyer/WarningsOff.h>
    std::string name_;                                  // argument name for synopsis
    ValueParser::Ptr parser_;                           // how to match and parse this argument
    ParsedValue defaultValue_;                          // default value if the argument is optional
#include <Sawyer/WarningsRestore.h>
public:
    /** Construct a new required argument. The @p name is used in documentation and error messages and need not be
     *  unique. */
    explicit SwitchArgument(const std::string &name, const ValueParser::Ptr &parser = anyParser())
        : name_(name), parser_(parser) {}

    /** Construct a new switch optional argument.  The @p name is used in documentation and error messages and need not be
     *  unique. The @p defaultValueString is immediately parsed via supplied parser and stored; an
     *  <code>std::runtime_error</code> exception is thrown if it cannot be parsed. */
    SwitchArgument(const std::string &name, const ValueParser::Ptr &parser, const std::string &defaultValueString)
        : name_(name), parser_(parser), defaultValue_(parser->matchString(defaultValueString)) {
        defaultValue_.valueLocation(NOWHERE);
    }

    /** Returns true if this argument is required. An argument is a required argument if it has no default value. */
    bool isRequired() const {
        return defaultValue_.isEmpty();
    }

    /** Returns true if this argument is not required.  Any argument that is not a required argument will have a default value
     *  that is returned in its place when parsing the switch. */
    bool isOptional() const {
        return !isRequired();
    }

    /** Argument name. The name is used for documentation and error messages.  It need not look like a variable. For instance,
     *  it could be "on|off|auto" to indicate that three values are possible, or "\@v{int}|wide" to indicate that the value can
     *  be any integer or the string "wide".  The string may contain markup, which is removed when used in error messages. As a
     *  convenience, if the string begins with a lower-case letter and contains only lower-case letters, hyphens, and
     *  underscores then it will be treated as a variable.  That is, the string "foo" is shorthand for "\@v{foo}". */
    const std::string &name() const { return name_; }

    /** Returns the name without markup. @sa name */
    std::string nameAsText() const;

    /** The parsed default value. The ParsedValue::isEmpty() will return true if this argument is required. */
    const ParsedValue& defaultValue() const {
        return defaultValue_;
    }

    /** The default value string.  This is the string that was parsed to create the default value.  Returns an empty string if
     *  the argument is required, but since such values are also valid default values one should call isRequired() or
     *  isOptional() to make that determination. */
    const std::string& defaultValueString() const {
        return defaultValue_.string();
    }

    /** Returns a pointer to the parser.  Parsers are reference counted and should not be explicitly destroyed. */
    const ValueParser::Ptr& parser() const { return parser_; }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch actions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for switch actions.
 *
 *  These objects represent some action that occurs after all switches are parsed and the ParserResult::apply method is
 *  called.
 *
 *  Actions are always allocated on the heap and reference counted.  Each action defines a class factory method,
 *  <code>instance</code>, to allocate a new object and return a pointer to it. The pointer types are named <code>Ptr</code>
 *  and are defined within the class.  For convenience, the actions built into the library also have global factory functions
 *  which have the same name as the class but start with an initial lower-case letter (see @ref sawyer_action_factories for
 *  details).  For instance:
 *
 * @code
 *  SwitchAction::Ptr action1 = UserAction::instance<MyFunctor>(myFunctor);
 *  SwitchAction::Ptr action2 = userAction(myFunctor);
 * @endcode */
class SAWYER_EXPORT SwitchAction: public SharedObject {
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<SwitchAction> Ptr;
    virtual ~SwitchAction() {}

    /** Runs the action.  Calling this method will cause the function operator to be invoked with the parser results. */
    void run(const ParserResult &parserResult) /*final*/ { (*this)(parserResult); }
protected:
    virtual void operator()(const ParserResult&) = 0;
};

/** Functor to print a version string. The string supplied to the constructor is printed to standard error followed by a line
 *  feed. Although the string is intended to be a version number, it can be anything you like. Sometimes people use this action
 *  to aid debugging of the parsing. */
class SAWYER_EXPORT ShowVersion: public SwitchAction {
#include <Sawyer/WarningsOff.h>
    std::string versionString_;
#include <Sawyer/WarningsRestore.h>
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    explicit ShowVersion(const std::string &versionString): versionString_(versionString) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ShowVersion> Ptr;

    /** Allocating constructor. Returns a pointer to a new ShowVersion object.  Uses will most likely want to use the @ref
     *  showVersion factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(const std::string &versionString) { return Ptr(new ShowVersion(versionString)); }
protected:
    virtual void operator()(const ParserResult&) /*overload*/;
};

/** Functor to print a version string and exit. This functor does the same thing as ShowVersion, but then it exits the program
 *  with the specified status. */
class SAWYER_EXPORT ShowVersionAndExit: public ShowVersion {
    int exitStatus_;
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    explicit ShowVersionAndExit(const std::string &versionString, int exitStatus)
        : ShowVersion(versionString), exitStatus_(exitStatus) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ShowVersionAndExit> Ptr;

    /** Allocating constructor. Returns a pointer to a new ShowVersionAndExit object.  Uses will most likely want to use the
     *  @ref showVersionAndExit factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(const std::string &versionString, int exitStatus) {
        return Ptr(new ShowVersionAndExit(versionString, exitStatus));
    }
protected:
    virtual void operator()(const ParserResult&) /*overload*/;
};

/** Functor to print the Unix man page.  This functor, when applied, creates a Unix manual page from available documentation in
 *  the parser, and then invokes standard Unix commands to format the text and page it to standard output. Strange things might
 *  happen if standard input is not the terminal, since the pager will probably be expecting to read commands from standard
 *  input. */
class SAWYER_EXPORT ShowHelp: public SwitchAction {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    ShowHelp() {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ShowHelp> Ptr;

    /** Allocating constructor. Returns a pointer to a new ShowHelp object.  Uses will most likely want to use the @ref
     *  showHelp factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance() { return Ptr(new ShowHelp); }
protected:
    virtual void operator()(const ParserResult&) /*override*/;
};

/** Functor to print the Unix man page and exit.  This functor is the same as ShowHelp except it also exits with the specified
 *  value. */
class SAWYER_EXPORT ShowHelpAndExit: public ShowHelp {
    int exitStatus_;
protected:
    /** Constructor for derived classes.  Non-subclass users should use @ref instance instead. */
    ShowHelpAndExit(int exitStatus): exitStatus_(exitStatus) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ShowHelpAndExit> Ptr;

    /** Allocating constructor.  Returns a pointer to a new ShowHelpAndExit object.  Users will most likely want to use the
     * @ref showHelpAndExit factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(int exitStatus) { return Ptr(new ShowHelpAndExit(exitStatus)); }
protected:
    virtual void operator()(const ParserResult&) /*override*/;
};

/** Functor to configure diagnostics.  This functor uses the string(s) from the specified switch key to configure the specified
 *  message facilities object.  If a string is the word "list" then the message facility configuration is shown on standard
 *  output.  If the string is the word "help" then some documentation is emitted on <code>std::cout</code> and the program
 *  optionally exits with success (depending on @p exitOnHelp).
 *
 *  Here's an example usage.  In particular, be sure to specify SAVE_ALL so that more than one <code>--log</code> switch can
 *  appear on the command line, like: <tt>a.out --log ">=info" --log list</tt>
 *
 * @code
 *  generic.insert(Switch("log")
 *                 .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
 *                 .argument("config")
 *                 .whichValue(SAVE_ALL)
 *                 .doc("Configures diagnostics..."));
 * @endcode */
class SAWYER_EXPORT ConfigureDiagnostics: public SwitchAction {
#include <Sawyer/WarningsOff.h>
    std::string switchKey_;
    Message::Facilities &facilities_;
    bool exitOnHelp_;
#include <Sawyer/WarningsRestore.h>
protected:
    /** Constructor for derived classes.  Non-subclass users should use @ref instance instead. */
    ConfigureDiagnostics(const std::string &switchKey, Message::Facilities &facilities, bool exitOnHelp)
        : switchKey_(switchKey), facilities_(facilities), exitOnHelp_(exitOnHelp) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ConfigureDiagnostics> Ptr;

    /** Allocating constructor.  Returns a pointer to a new ConfigureDiagnostics object.  Users will most likely want to use
     * the @ref configureDiagnostics factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(const std::string &switchKey, Message::Facilities &facilities, bool exitOnHelp=true) {
        return Ptr(new ConfigureDiagnostics(switchKey, facilities, exitOnHelp));
    }

    /** Property: program exit after help is displayed.
     *
     *  If true, then the program exits with success immediately after a "help" command is processed.
     *
     *  @{ */
    bool exitOnHelp() const { return exitOnHelp_; }
    void exitOnHelp(bool b) { exitOnHelp_=b; }
    /** @} */
protected:
    virtual void operator()(const ParserResult&) /*override*/;
};

/** Function to configure diagnostics to quiet mode.
 *
 *  @code
 *   generic.insert(Switch("quiet", 'q')
 *                  .action(configureDiagnosticsQuiet(Sawyer::Message::mfacilities))
 *                  .doc("Turn off all diagnostic output except error and fatal levels."));
 *  @endcode */
class SAWYER_EXPORT ConfigureDiagnosticsQuiet: public SwitchAction {
#include <Sawyer/WarningsOff.h>
    Message::Facilities &facilities_;
#include <Sawyer/WarningsRestore.h>
protected:
    ConfigureDiagnosticsQuiet(Message::Facilities &facilities)
        : facilities_(facilities) {
    }

public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ConfigureDiagnosticsQuiet> Ptr;

    /** Allocating constructor. Returns a pointer to a new ConfigureDiagnosticsQuiet object. Users will most likely want to use
     * the @ref configureDiagnosticsQuiet factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(Message::Facilities &facilities) {
        return Ptr(new ConfigureDiagnosticsQuiet(facilities));
    }

protected:
    virtual void operator()(const ParserResult&) /*override*/;
};

/** Wrapper around a user functor.  User code doesn't often use reference counting smart pointers for functors, but more often
 *  creates functors in global data or on the stack.  The purpose of UserAction is to be a wrapper around these functors, to
 *  be a bridge between the world of reference counting pointers and objects or object references.  For example, say the
 *  user has a function like this:
 *
 * @code
 *  void showRawManPage(const ParserResult *cmdline) {
 *      std::cout <<cmdline->parser()->manpage();
 *      exit(0);
 *  }
 * @endcode
 *
 *  Then the functor/function can be used like this:
 *
 * @code
 *  switchGroup.insert(Switch("raw-man")
 *                     .action(userAction(showRawManPage))
 *                     .doc("Prints the raw nroff man page to standard output."));
 * @endcode */
template<class Functor>
class UserAction: public SwitchAction {
    Functor &functor_;
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    UserAction(Functor &f): functor_(f) {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<class UserAction> Ptr;

    /** Allocating constructor. Returns a pointer to a new UserAction object.  Uses will most likely want to use the @ref
     *  userAction factory instead, which requires less typing.
     *
     * @sa @ref sawyer_action_factories, and the @ref SwitchAction class. */
    static Ptr instance(Functor &f) { return Ptr(new UserAction(f)); }
protected:
    virtual void operator()(const ParserResult &parserResult) /*override*/ { (functor_)(parserResult); }
};

/** @defgroup sawyer_action_factories Command line action factories
 *  @ingroup sawyer
 *
 *  Factories for creating instances of Sawyer::CommandLine::SwitchAction subclasses.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for the various
 *  subclasses of @ref SwitchAction, and they sometimes alleviate the user from having to supply template arguments.  They take
 *  the same arguments as the @c instance method(s) and have the same name as the class they return, except their initial
 *  letter is lower-case. These two calls are equivalent:
 *
 * @code
 *  SwitchAction::Ptr action1 = UserAction::instance<MyFunctor>(myFunctor);
 *  SwitchAction::Ptr action2 = userAction(myFunctor);
 * @endcode
 *
 * @section factories Factories */

/** @ingroup sawyer_action_factories
 *  @brief Factory for switch action.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for @ref
 *  SwitchAction subclasses and often alleviates the user from having to specify template arguments.
 *
 * @sa
 *  @li @ref sawyer_action_factories
 *  @li @ref SwitchAction base class
 *  @li Documentation for the returned class.
 * @{ */

SAWYER_EXPORT ShowVersion::Ptr showVersion(const std::string &versionString);
SAWYER_EXPORT ShowVersionAndExit::Ptr showVersionAndExit(const std::string &versionString, int exitStatus);

SAWYER_EXPORT ShowHelp::Ptr showHelp();

SAWYER_EXPORT ShowHelpAndExit::Ptr showHelpAndExit(int exitStatus);

SAWYER_EXPORT ConfigureDiagnostics::Ptr configureDiagnostics(const std::string&, Message::Facilities&, bool exitOnHelp=true);

SAWYER_EXPORT ConfigureDiagnosticsQuiet::Ptr configureDiagnosticsQuiet(Message::Facilities&);

template<class Functor>
typename UserAction<Functor>::Ptr userAction(const Functor &functor) {
    return UserAction<Functor>::instance(functor);
}
/** @} */



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch value agumenters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for value agumentors.
 *
 *  A ValueAugmenter is invoked after a switch argument (explicit or default) is parsed and somehow merges the newly parsed
 *  value with previous values.  The augmenter is called only when the switch @c whichProperty is set to @ref SAVE_AUGMENTED.
 *
 *  The merge is performed by the function operator, which is given two sets of parsed values: the values that were previously
 *  parsed and stored in a ParserResult for the same switch key, and the values that were recently parsed for the switch.  The
 *  method should combine the two in some way and return a new set of values that replace all previous and recently parsed
 *  values.  The return values should contain appropriate information for the @c valueLocation, @c valueString, @c
 *  switchLocation, @c switchString, and @c valueSaver properties.  Most of the time these properties can be initialized from
 *  parsed values passed to the function operator.
 *
 *  Most subclasses will have factory functions to instantiate reference counted, allocated objects. See @ref
 *  sawyer_augmenter_factories for a list. */
class SAWYER_EXPORT ValueAugmenter: public SharedObject {
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<ValueAugmenter> Ptr;
    virtual ~ValueAugmenter() {}

    /** Called when a switch's value is about to be stored into the ParserResult.  The previously stored switch values for all
     * switch occurrences that used this same key are provided in the first arugment.  The recently parsed value (or values if
     * the switch value was a list that was then exploded) is provided as the second argument.  The function should return a
     * new value (or values) that will replace the values passed on the command-line. */
    virtual ParsedValues operator()(const ParsedValues &savedValues, const ParsedValues &newValues) = 0;
};

/** Sums all previous and current values. The template argument must match the type of the summed values.
 *
 *  This augmenter can be used for switches that increment a value, such as a debug switch where each occurrence of the switch
 *  increments a debug level:
 *
 * @code
 *  int debugLevel = 0;
 *  switchGroup.insert(Switch("debug", "d")
 *                     .intrinsicValue("1", integerParser(debugLevel))
 *                     .valueAugmenter(sum<int>())
 *                     .whichValue(SAVE_AUGMENTED));
 * @endcode */
template<typename T>
class Sum: public ValueAugmenter {
protected:
    /** Constructor for derived classes. Non-subclass users should use @ref instance instead. */
    Sum() {}
public:
    /** Reference counting pointer for this class. */
    typedef SharedPointer<Sum> Ptr;

    /** Allocating constructor. Returns a pointer to a new Sum object.  Uses will most likely want to use the @ref
     *  sum factory instead, which requires less typing.
     *
     * @sa @ref sawyer_augmenter_factories, and the @ref ValueAugmenter class. */
    static Ptr instance() { return Ptr(new Sum<T>); }

    virtual ParsedValues operator()(const ParsedValues &savedValues, const ParsedValues &newValues) /*override*/ {
        ASSERT_forbid(newValues.empty());
        T sum = 0;
        BOOST_FOREACH (const ParsedValue &pv, savedValues)
            sum = sum + boost::any_cast<T>(pv.value());
        BOOST_FOREACH (const ParsedValue &pv, newValues)
            sum = sum + boost::any_cast<T>(pv.value());
        ParsedValue pval = newValues.front();           // so we keep the same location information
        pval.value(sum);
        ParsedValues pvals;
        pvals.push_back(pval);
        return pvals;
    }
};

/** @defgroup sawyer_augmenter_factories Command line value augmenter factories
 *  @ingroup sawyer
 *
 *  Factories for creating instances of Sawyer::CommandLine::ValueAugmenter subclasses.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for the various
 *  subclasses of @ref ValueAugmenter, and they sometimes alleviate the user from having to supply template arguments.  They
 *  take the same arguments as the @c instance method(s) and have the same name as the class they return, except their initial
 *  letter is lower-case.  These two calls are equivalent:
 *
 * @code
 *  ValueAugmenter::Ptr a1 = Sum::instance<int>();
 *  ValueAugmenter::Ptr a2 = sum<int>();
 * @endcode
 *
 * @section factories Factories */

/** @ingroup sawyer_augmenter_factories
 *  @brief Factory for value agumenter.
 *
 *  A factory function is a more terse and convenient way of calling the @c instance allocating constructors for @ref
 *  ValueAugmenter subclasses and often alleviates the user from having to specify template arguments.
 *
 * @sa
 *  @li @ref sawyer_augmenter_factories
 *  @li @ref ValueAugmenter base class
 *  @li Documentation for the returned class.
 *
 * @{ */
template<typename T>
typename Sum<T>::Ptr sum() {
    return Sum<T>::instance();
}
/** @} */



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Used internally to pass around switch properties that are common among parsers, switch groups, and switches.
struct SAWYER_EXPORT ParsingProperties {
#include <Sawyer/WarningsOff.h>
    std::vector<std::string> longPrefixes;              // Prefixes for long command-line switches
    bool inheritLongPrefixes;                           // True if this also inherits longPrefixes from a higher layer
    std::vector<std::string> shortPrefixes;
    bool inheritShortPrefixes;
    std::vector<std::string> valueSeparators;           // What separates a long switch from its value.
    bool inheritValueSeparators;
    ShowGroupName showGroupName;                        // How to show group name in switch synopsis
#include <Sawyer/WarningsRestore.h>
    ParsingProperties()
        : inheritLongPrefixes(true), inheritShortPrefixes(true), inheritValueSeparators(true),
          showGroupName(SHOW_GROUP_INHERIT) {}
    ParsingProperties inherit(const ParsingProperties &base) const;
};

/** Describes how to handle switches that occur multiple times. */
enum WhichValue {
    SAVE_NONE,                                          /**< The switch is disabled. Any occurrence will be an error. */
    SAVE_ONE,                                           /**< The switch cannot occur more than once. */
    SAVE_LAST,                                          /**< Use only the last occurrence and ignore all previous. */
    SAVE_FIRST,                                         /**< Use only the first occurrence and ignore all previous. */
    SAVE_ALL,                                           /**< Save all values as a vector. */
    SAVE_AUGMENTED                                      /**< Save the first value, or modify previously saved value. */
};

/** Describes one command-line switch.
 *
 *  A command line switch is something that typically starts with a hyphen on Unix or a slash on Windows, followed by a name,
 *  and zero or more values.  Switches refer to named command-line arguments but not usually positional command-line
 *  arguments.
 *
 *  The user normally interacts with the Switch class by instantiating a new switch object and setting properties for that
 *  object before inserting it into a SwitchGroup.  All property-setting methods return a reference to the same switch and are
 *  typically chained together. In fact, the switch is often immediately inserted into the SwitchGroup without even creating a
 *  permanent switch object:
 *
 * @code
 *  SwitchGroup outputSwitches;
 *  outputSwitches.insert(Switch("use-color", 'C')
 *                        .argument("when",                         // argument name
 *                                  (enumParser<WhenColor>()        // WhenColor must be declared at global scope
 *                                   ->with("never",  NEVER)        // possible values
 *                                   ->with("auto",   AUTO)
 *                                   ->with("always", ALWAYS)),
 *                                  "always"));                     // the default value
 * @endcode
 */
class SAWYER_EXPORT Switch {
private:
#include <Sawyer/WarningsOff.h>
    std::vector<std::string> longNames_;                // Long name of switch, or empty string.
    std::string shortNames_;                            // Optional short names for this switch.
    std::string key_;                                   // Unique key, usually the long name or the first short name.
    ParsingProperties properties_;                      // Properties valid at multiple levels of the hierarchy.
    std::string synopsis_;                              // User-defined synopsis or empty string.
    std::string documentation_;                         // Main documentation for the switch.
    std::string documentationKey_;                      // For sorting documentation.
    bool hidden_;                                       // Whether to hide documentation.
    std::vector<SwitchArgument> arguments_;             // Arguments with optional default values.
    SwitchAction::Ptr action_;                          // Optional action to perform during ParserResult::apply.
    WhichValue whichValue_;                             // Which switch values should be saved.
    ValueAugmenter::Ptr valueAugmenter_;                // Used if <code>whichValue_==SAVE_AUGMENTED</code>.
    ParsedValue intrinsicValue_;                        // Value for switches that have no declared arguments.
    bool explosiveLists_;                               // Whether to expand ListParser::ValueList into separate values.
    SwitchSkipping skipping_;                           // Whether to skip over this switch without saving or acting.
#include <Sawyer/WarningsRestore.h>

public:
    /** Constructs a switch declaration.  Every switch must have either a long or short name (or both), neither of which should
     *  include prefix characters such as hyphens. The best practice is to provide a long name for every switch and short names
     *  only for the most commonly used switches.
     *
     *  Additional names can be specified with the @ref longName and @ref shortName methods, but the constructor should be
     *  provided the canonical names.  The canonical names will be used to create the initial value for the @ref key property,
     *  and will be used for organizing parsed values by switch. They also appear in some error messages when command-line text
     *  is not available, and they will appear first in documentation.
     *
     *  Names of switches need not be unique.  If more than one switch is able to parse a command-line argument then the first
     *  declaration wins.  This feature is sometimes used to declare two switches having identical names but different
     *  arguments or argument types. */
    explicit Switch(const std::string &longName, char shortName='\0')
        : hidden_(false), whichValue_(SAVE_LAST), intrinsicValue_(ParsedValue(true, NOWHERE, "true", ValueSaver::Ptr())),
          explosiveLists_(false), skipping_(SKIP_NEVER) {
        init(longName, shortName);
    }

    // FIXME[Robb Matzke 2014-03-03]: test that "--prefix=5" works when names are "pre" and "prefix" in that order
    /** Property: switch long name.  Long names normally consist of multiple letters and use prefixes that are different than
     *  the short (single-letter) names.  Also, the value for long-name switches is usually separated from the switch by a
     *  special character (like "=") or by appearing in the next program argument.
     * @{ */
    Switch& longName(const std::string &name);
    const std::string& longName() const { return longNames_.front(); }
    const std::vector<std::string>& longNames() const { return longNames_; }
    /** @} */

    /** Property: switch short name. Short names are single letters and use a different set of prefixes than long names (which
     *  can also be a single letter).  Also, when a short-name switch has an argument the value for the argument can be
     *  adjacent to the switch, like <code>-n5</code>, but long-name switches usually separate the switch from the value with a
     *  special string, like <code>-\-number=5</code>.
     * @{ */
    Switch& shortName(char c) { if (c) shortNames_ += std::string(1, c); return *this; }
    const std::string& shortNames() const { return shortNames_; }
    /** @} */

    /** Name by which switch prefers to be known.  This is the first long name, or the first short name if there are no
     *  long names. */
    std::string preferredName() const { return longNames_.empty() ? std::string(1, shortNames_[0]) : longNames_[0]; }

    /** Property: value storage key.  When a switch value is parsed (or an intrinsic or default value is used) to create a
     *  ParsedValue object, the object will be associated with the switch key.  This allows different switches to write to the
     *  same result locations and is useful for different keys that refer to the same concept, like <code>-\-verbose</code> and
     *  <code>-\-quiet</code>. The default is to use the long (or short, if no long) name specified in the constructor.  The
     *  switch prefix (e.g., "-") should generally not be part of the key because it becomes confusing when it appears in error
     *  messages (it looks like a switch occurrence string yet might be completely unrelated to what appeared on the command
     *  line).
     * @{ */
    Switch& key(const std::string &s) { key_ = s; return *this; }
    const std::string &key() const { return key_; }
    /** @} */

    /** Property: abstract summary of the switch syntax.  The synopsis is normally generated automatically from other
     *  information specified for the switch, but the user may provide a synopsis to override the generated one.  A synopsis
     *  should be a semi-colon separated list of alternative switch sytax specifications using markup to specify things such as
     *  the switch name and switch/value separator.  Using markup will cause the synopsis to look correct regardless of the
     *  operating system or output media.  See the @ref doc method for details.  For example, the git-rev-parse(1) man page
     *  has the following documentation for three related switches:
     *
     * @code
     *  --branches[=PATTERN], --tags[=PATTERN], --remotes[=PATTERN]
     *      Show all branches, tags, or remote-tracking branches, respecitively (i.e., refs
     *      found in refs/heads, refs/tags, or refs/remotes, respectively).  If a pattern is
     *      given, only refs matching the given shell glob are shown.  If the pattern does
     *      not contain a globbing character (?, *, or [), it is turned into a prefix match
     *      by appending a slash followed by an asterisk.
     * @endcode
     *
     *  This library handles that situation by attaching the documentation to a single switch, say "branches", and hiding
     *  documentation for the other two switches.  But this also requires that the synopsis for "branches" be changed from
     *  its library-generated vesion:
     *
     * @code
     *  Switch("branches")
     *         .doc("@s{branches} [@v{pattern}]; @s{tags} [@v{pattern}]; @s{remotes} [@v{pattern}]")
     * @endcode
     *
     *  Note: the git man pages separate alternative switches with commas, but this library uses semicolons since commas also
     *  frequirently appear within the syntax description of a single switch.
     *
     * @sa @ref doc
     *
     * @{ */
    Switch& synopsis(const std::string &s) { synopsis_ = s; return *this; }
    std::string synopsis() const;
    /** @} */

    /** Property: detailed description.  This is the description of the switch in a simple markup language.
     *
     *  Parts of the text can be marked by surrounding the text in curly braces and prepending a tag consisting of an "@"
     *  followed by a word.  For instance, <code>\@b{foo}</code> makes the word "foo" bold and <code>\@i{foo}</code> makes it
     *  italic.  The text between the curly braces can be any length, and if it contains curly braces they must either balance
     *  or be escaped with a preceding "@". The curly braces that start an argument must not be preceded by white space.
     *
     *  Besides describing the format of a piece of text, markup is also used to describe the intent of a piece of text.  The
     *  following intents are supported:
     *
     *  @li @c @@v{foo} specifies that "foo" is a variable. POD-based man pages will underline it, and text based documentation
     *      will surround it with angle brackets.
     *  @li @c @@s{foo} specifies that "foo" is a switch. The argument should be a single word or single letter without leading
     *      hyphens, and the correct prefix (hyphens, etc.) will be added automatically based on the parser's
     *      configuration. The documentation system also verifies that "foo" is spelled correctly and if not, it adds an error
     *      to the "Documentation issues" section of the output.  To suppress these errors, supply a second argument whose
     *      name is "noerror", as in "@s{foo}{noerror}".
     *  @li @c @@man{foo}{n} means refer to the "foo" page in chapter "n" of the man pages. This also adds an entry to the "See
     *      also" section of the output.
     *
     *  The <code>\@prop</code> tag takes one argument which is a property name and is replaced by the value of the property.
     *  The following properties are defined:
     *
     *  @li @c inclusionPrefix is the preferred (first) string returned by Parser::inclusionPrefixes.
     *  @li @c terminationSwitch is the preferred (first) switch returned by Parser::terminationSwitches.
     *  @li @c programName is the string returned by Parser::programName.
     *  @li @c purpose is the string returned by Parser::purpose.
     *  @li @c versionString is the first member of the pair returned by Parser::version.
     *  @li @c versionDate is the second member of the pair returned by Parser::version.
     *  @li @c chapterNumber is the first member of the pair returned by Parser::chapter.
     *  @li @c chapterName is the second member of the pair returned by Parser::chapter.
     *
     *  Even switches with no documentation will show up in the generated documentation--they will be marked as "Not
     *  documented".  To suppress them entirely, set their @ref hidden property to true.
     * @{ */
    Switch& doc(const std::string &s) { documentation_ = s; return *this; }
    const std::string& doc() const { return documentation_; }
    /** @} */

    /** Property: key to control order of documentation.  Normally, documentation for a group of switches is sorted according
     *  to the switche's @ref preferredName.  Specifying a docKey string causes that string to be used instead of the switch
     *  names.  The key string itself never appears in any documentation.
     *  @{ */
    Switch& docKey(const std::string &s) { documentationKey_ = s; return *this; }
    const std::string &docKey() const { return documentationKey_; }
    /** @} */

    /** Property: whether this switch appears in documentation. A hidden switch still participates when parsing command lines,
     *  but will not show up in documentation.  This is ofen used for a switch when that switch is documented as part of some
     *  other switch.
     * @{ */
    Switch& hidden(bool b) { hidden_ = b; return *this; }
    bool hidden() const { return hidden_; }
    /** @} */

    /** Property: whether to skip over this switch.
     *
     *  The default is to not skip over anything, in which case if the switch appears on the command-line its actions are run
     *  and its value are saved.  If skipping is set to @ref SKIP_WEAK or @ref SKIP_STRONG then the switch and its arguments
     *  are also added to the skipped list returned by @ref ParserResult::skippedArgs and @ref ParserResult::unparsedArgs.  The
     *  difference between weak and strong is that strong also skips any actions and value saving for the switch.
     *
     *  For short, nestled switches, a program argument is added to the skipped list if any of the short switches in that
     *  argument are @ref SKIP_WEAK or @ref SKIP_STRONG.
     *
     * @{ */
    Switch& skipping(SwitchSkipping how) { skipping_ = how; return *this; }
    SwitchSkipping skipping() const { return skipping_; }
    /** @} */

    /** Property: prefixes for long names.  A long name prefix is the characters that introduce a long switch, usually "-\-"
     *  and sometimes "-" or "+"). Prefixes are specified in the parser, the switch group, and the switch, each entity
     *  inheriting and augmenting the list from the earlier entity.  To prevent inheritance use the @ref resetLongPrefixes
     *  method, which also takes an optional list of new prefixes.  In any case, additional prefixes can be added with @ref
     *  longPrefix, which should be after @ref resetLongPrefixes if both are used.  The empty string is a valid prefix and is
     *  sometimes used in conjunction with "=" as a value separator to describe dd(1)-style switches of the form "bs=1024".
     *
     *  It is generally unwise to override prefixes without inheritance since the parser itself chooses the basic prefixes
     *  based on the conventions used by the operating system.  Erasing these defaults might get you a command line syntax
     *  that's jarring to uses.
     * @{ */
    Switch& resetLongPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                              const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Switch& longPrefix(const std::string &s1) { properties_.longPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& longPrefixes() const { return properties_.longPrefixes; }
    /** @} */

    /** Property: prefixes for short names.  A short name prefix is the characters that introduce a short switch, usually
     *  "-". Prefixes are specified in the parser, the switch group, and the switch, each entity inheriting and augmenting the
     *  list from the earlier entity.  To prevent inheritance use the @ref resetShortPrefixes method, which also takes an
     *  optional list of new prefixes.  In any case, additional prefixes can be added with @ref shortPrefix, which should be
     *  after @ref resetShortPrefixes if both are used.  The empty string is a valid short prefix to be able to parse
     *  tar(1)-like switches like "xvf".
     *
     *  It is generally unwise to override prefixes without inheritance since the parser itself chooses the basic prefixes
     *  based on the conventions used by the operating system.  Erasing these defaults might get you a command line syntax
     *  that's jarring to uses.
     * @{ */
    Switch& resetShortPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                               const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Switch& shortPrefix(const std::string &s1) { properties_.shortPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& shortPrefixes() const { return properties_.shortPrefixes; }
    /** @} */

    /** Property: strings that separate a long switch from its value. A value separator is the string that separates a long
     *  switch name from its value and is usually "=" and/or " ".  The " " string has special meaning: it indicates that the
     *  value must be separated from the switch by being in the following command line argument.  Separators are specified in
     *  the parser, the switch group, and the switch, each entity inheriting and augmenting the list from the earlier entity.
     *  To prevent inheritance use the @ref resetValueSeparators method, which also takes an optional list of new separators.
     *  In any case, additional separators can be added with @ref valueSeparator, which should be after @ref
     *  resetValueSeparators if both are used.
     *
     *  The empty string is a valid separator although typically not used since it leads to hacks like <code>-\-liberty</code>
     *  as an alternative to <code>-\-lib=irty</code>, and confusing error messages like "switch '-\-lib' is mutually exclusive
     *  with '-\-no-libs'" (and the user says, "I didn't use '-\-lib' anywhere, something's wrong with the parser").
     *
     *  It is generally unwise to override the separators without inheritance since the parser itself chooses the basic
     *  separators based on the conventions used by the operating system.  Erasing these defaults might get you a command line
     *  syntax that's jarring to uses.
     * @{ */
    Switch& resetValueSeparators(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                 const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Switch& valueSeparator(const std::string &s1) { properties_.valueSeparators.push_back(s1); return *this; }
    const std::vector<std::string>& valueSeparators() const { return properties_.valueSeparators; }
    /** @} */

    /** Property: switch argument.  A switch argument declares how text after the switch name is parsed to form a value.  An
     *  argument specifier contains a name, a parser, and an optional default value.
     *
     *  The @p name is used in error messages and documentation and should be chosen to be a descriptive but terse. Good
     *  examples are (here juxtaposed with their switch): -\-lines=<em>n</em>, -\-author=<em>name</em>, -\-image-type=rgb|gray.
     *  The string may contain simple markup which is removed when used in error messages. As a convenience, if the string
     *  looks like a lower-case variable name then it will be formatted in the documentation like a variable.
     *
     *  The value @p parser is normally created with one of the @ref sawyer_parser_factories.  It defaults to the @ref
     *  AnyParser, which accepts any string from the command line and stores it as an <code>std::string</code>.
     *
     *  If a default is specified then the argument will be optional and the parser will behave as if the default value string
     *  appeared on the command line at the point where the argument was expected.  This also means that the default value
     *  string must be parsable as if it were truly present.
     *
     *  Although switches usually have either no arguments or one argument, it is possible to declare switches that have
     *  multiple arguments.  This is different from a switch that occurs multiple times or from a switch that has one argument
     *  which is a list.  Each switch argument must be separated from the previous switch argument by occuring in a subsequent
     *  program command line argument.  E.g., <code>-\-swap a b</code> as three program arguments, or <code>-\-swap=a b</code> as
     *  two program arguments; but <code>-\-swap=a,b</code> is one switch argument that happens to look like a list, and
     *  <code>-\-swap=a -\-swap=b</code> is obviously two switches with one switch argument each.
     * @{ */
    Switch& argument(const std::string &name, const ValueParser::Ptr &parser = anyParser());
    Switch& argument(const std::string &name, const ValueParser::Ptr &parser, const std::string &defaultValue);
    Switch& argument(const SwitchArgument &arg) { arguments_.push_back(arg); return *this; }
    const SwitchArgument& argument(size_t idx) const { return arguments_[idx]; }
    const std::vector<SwitchArgument>& arguments() const { return arguments_; }
    /** @} */

    /** Total number of arguments.  This is the total number of arguments currently declared for the switch. */
    size_t nArguments() const { return arguments_.size(); }

    /** Number of required arguments. This is the number of arguments which do not have default values and which therefore must
     *  appear on the command line for each occurrence of the switch. */
    size_t nRequiredArguments() const;

    /** Property: value for a switch that has no declared arguments.  A switch with no declared arguments (not even optional
     *  arguments) is always parsed as if it had one argument--it's intrinsic value--the string "true" parsed and stored as
     *  type "bool" by the @ref BooleanParser.  The intrinsicValue property can specify a different value and type for the
     *  switch.  For instance, <code>-\-laconic</code> and <code>-\-effusive</code> might be two switches that use the same key
     *  "verbosity" and have intrinsic values of "1" and "2" as integers (or even @c LACONIC and @c EFFUSIVE as enums).
     *
     *  Intrinsic values can either be supplied as a string and a parser, or as the value itself.  If a value is supplied
     *  directly, then it must have a printing operator (<code>operator<<(std::ostream, const T&)</code>) so that its
     *  string representation can be returned if the user asks the ParserResult for it.
     *
     *  If a switch has at least one declared argument then this property is not consulted, even if that argument is optional
     *  and missing (in which case that argument's default value is used).
     *
     *  Some examples:
     *
     * @code
     *  bool b1, b2;
     *  Switch("bool1").intrinsicValue("true", booleanParser(b1));
     *  Switch("bool2").intrinsicValue(true, b2);
     *
     *  int i1, i2;
     *  Switch("int1").intrinsicValue("123", integerParser(i1));
     *  Switch("int2").intrinsicValue(123, i2);
     *
     *  std::string s1, s2;
     *  Switch("str1").intrinsicValue("hello world", anyParser(s1));
     *  Switch("str2").intrinsicValue(std::string("hello world"), s2);
     * @endcode
     * @{ */
    template<typename T>
    Switch& intrinsicValue(const T &value, T &storage) {
        intrinsicValue_ = ParsedValue(value, NOWHERE, boost::lexical_cast<std::string>(value), TypedSaver<T>::instance(storage));
        return *this;
    }
    Switch& intrinsicValue(const char *value, std::string &storage) {
        intrinsicValue_ = ParsedValue(std::string(value), NOWHERE, value, TypedSaver<std::string>::instance(storage));
        return *this;
    }
    Switch& intrinsicValue(const std::string &text, const ValueParser::Ptr &p) {
        intrinsicValue_ = p->matchString(text);
        intrinsicValue_.valueLocation(NOWHERE);
        return *this;
    }
    Switch& intrinsicValue(const std::string &text) {
        intrinsicValue_ = anyParser()->matchString(text);
        intrinsicValue_.valueLocation(NOWHERE);
        return *this;
    }
    Switch& intrinsicValue(const ParsedValue &value) { intrinsicValue_ = value; return *this; }
    ParsedValue intrinsicValue() const { return intrinsicValue_; }
    /** @} */

    /** Property: whether to convert a list value to individual values.  The @ref ListParser returns a single value of type
     *  ListParser::ValueList, an <code>std::list</code> containing parsed values.  If the explodeLists property is true, then
     *  each switch argument that is a ListParser::ValueList is exploded into multiple switch arguments.
     *
     *  This is useful for switches that can either occur multiple times on the command-line or take a list as an argument,
     *  such as GCC's <code>-I</code> switch, where <code>-Ia:b:c</code> is the same as <code>-Ia -Ib -Ic</code>.  It is also
     *  useful when one wants to store a list argument in a user-supplied <code>std::vector</code> during the
     *  ParserResult::apply call.  Here's an example that stores the aforementioned <code>-I</code> switch arguments in a
     *  vector:
     *
     * @code
     *  std::vector<std::string> includeDirectories;
     *  switchGroup.insert(Switch("incdir", 'I')
     *                     .argument("directories", listParser(anyParser(includeDirectories)))
     *                     .explosiveLists(true));
     * @endcode
     *
     *  If the explosiveLists property was false (its default) and the command line was <code>-Ia:b:c</code>, then the library
     *  would try to store a ListParser::ValueList as the first element of @c includeDirectories, resulting in an
     *  <code>std::runtime_error</code> regarding an invalid type conversion.
     *
     * @{ */
    Switch& explosiveLists(bool b) { explosiveLists_ = b; return *this; }
    bool explosiveLists() const { return explosiveLists_; }
    /** @} */

    /** Property: action to occur.  Each switch may define an action that will be called by ParserResult::apply. When a
     *  switch is recognized on the command line and added to the @ref ParserResult eventually returned by Parser::parse, an
     *  optional action functor is saved.
     *
     *  Actions are associted with the @ref key of parsed switches, with at most one action per key--the last action (or
     *  non-action if null) for a key is the one which is saved in the ParserResult for that key.  In other words, if two
     *  switches share the same key but have two different actions, the action for the switch that's parsed last will be the
     *  action called by ParserResult::apply.  A switch with a null action, when recognized on the command-line, cancels any
     *  action that was previously associated with the same key in the ParserResult from a previously recognized switch.
     *
     *  The library provides a few actions, all of which are derived from @ref SwitchAction, and the user can provide
     *  additional actions.  For instance, two commonly used switches are:
     *
     * @code
     *  sg.insert(Switch("help", 'h')             // allow "--help" and "-h"
     *            .shortName('?')                 // also "-?"
     *            .action(showHelp()))            // show documentation
     *  sg.insert(Switch("version", 'V')          // allow "--version" and "-V"
     *            .action(showVersion("1.2.3"))); // emit "1.2.3"
     * @endcode
     * @{ */
    Switch& action(const SwitchAction::Ptr &f) { action_ = f; return *this; }
    const SwitchAction::Ptr& action() const { return action_; }
    /** @} */

    /** Property: how to handle multiple occurrences. Describes what to do if a switch occurs more than once.  Normally, if a
     *  switch occurs more than once on the command line then only its final value is made available in the parser result since
     *  this is usually what one wants for most switches.  The @c whichValue property can be adjusted to change this behavior
     *  (see documentation for the @ref WhichValue enumeration for possibilities).  The @ref SAVE_AUGMENTED mode also needs a
     *  @ref valueAugmenter, otherwise it behaves the same as @ref SAVE_LAST.
     *
     *  The @c whichValue property is applied per switch occurrence, but applied using the switch's key, which it may share
     *  with other switches.  For example, if two switches, "foo1" and "foo2" both use the key "foo" and both have the @ref
     *  SAVE_ONE mode, then an error will be raised if the command line is <code>-\-foo1 -\-foo2</code>.  But if "foo1" uses @ref
     *  SAVE_ONE and "foo2" uses @ref SAVE_ALL, then no error is raised because at the point in the command line when the
     *  <code>-\-foo1</code> occurrence is checked, <code>-\-foo2</code> hasn't occurred yet.
     *
     *  A switch that has a list value (i.e., from @ref ListParser) is treated as having a single value per occurrence, but if
     *  the list is exploded (@ref explosiveLists) then it will be treated as if the switch occurred more than once.  In other
     *  words, @ref SAVE_ONE will not normaly raise an error for an occurrence like <code>-\-foo=a,b,c</code> because it's
     *  considered to be a single value (a single list), but would cause an error if the list were exploded into three values
     *  since the exploding makes the command line look more like <code>-\-foo=a -\-foo=b -\-foo=c</code>.
     *
     *  Single letter switches that are nestled and repeated, like the somewhat common debug switch where more occurrences
     *  means more debugging output, like <code>-d</code> for a little debugging versus <code>-ddd</code> for extensive
     *  debugging, are counted as individual switches: <code>-ddd</code> is equivalent to <code>-d -d -d</code>.
     *
     *  If a switch with @ref SAVE_LAST is processed, it deletes all the previously saved values for the same key even if they
     *  came from other switches having the same key but not the same @ref SAVE_LAST configuration.
     * @{ */
    Switch& whichValue(WhichValue s) { whichValue_ = s; return *this; }
    WhichValue whichValue() const { return whichValue_; }
    /** @} */

    /** Property: functor to agument values. This is the functor that is called to augment a previously parsed values by
     *  merging them with newly parsed values.  The @ref whichValue property must be @ref SAVE_AUGMENTED in order for the
     *  specified functor to be invoked.
     *
     * @sa
     *  @li @ref sawyer_augmenter_factories
     *  @li @ref ValueAugmenter base class
     * @{ */
    Switch& valueAugmenter(const ValueAugmenter::Ptr &f) { valueAugmenter_ = f; return *this; }
    ValueAugmenter::Ptr valueAugmenter() const { return valueAugmenter_; }
    /** @} */

public:
    // Used internally
    const ParsingProperties& properties() const { return properties_; }

private:
    friend class Parser;
    friend class ParserResult;

    void init(const std::string &longName, char shortName);

    /** @internal Constructs an exception describing that there is no separator between the switch name and its value. */
    std::runtime_error noSeparator(const std::string &switchString, const Cursor&, const ParsingProperties&) const;

    /** @internal Constructs an error describing extra text that appears after a switch. */
    std::runtime_error extraTextAfterSwitch(const std::string &switchString, const Location &endOfSwitch, const Cursor&,
                                            const ParsingProperties&, const ParsedValues&) const;

    /** @internal Constructs an exception describing that there is unexpected extra text after a switch argument. */
    std::runtime_error extraTextAfterArgument(const Cursor&, const ParsedValue &va) const;

    /** @internal Constructs an exception describing that we couldn't parse all the required arguments. */
    std::runtime_error notEnoughArguments(const std::string &switchString, const Cursor&, size_t nargs) const;

    /** @internal Constructs an exception describing an argument that is missing. */
    std::runtime_error missingArgument(const std::string &switchString, const Cursor &cursor,
                                       const SwitchArgument &sa, const std::string &reason) const;

    /** @internal Constructs an exception describing an argument that is malformed or invalid. */
    std::runtime_error malformedArgument(const std::string &switchString, const Cursor &cursor,
                                         const SwitchArgument &sa, const std::string &reason) const;

    /** @internal Determines if this switch can match against the specified program argument when considering only the
     *  specified long name.  If program argument starts with a valid long name prefix and then optionally matches the
     *  optionalPart (e.g., name space and name space separator) and then matches the required part (switch name), then this
     *  this function returns true (the number of characters matched).  Switches that take no arguments must match to the end
     *  of the string, but switches that have arguments (even if they're optional or wouldn't match the rest of the string) do
     *  not have to match entirely as long as a value separator is found when they don't match entirely. */
    size_t matchLongName(Cursor&/*in,out*/, const ParsingProperties &props,
                         const std::string &optionalPart, const std::string &requiredPart) const;

    /** @internal Matches a short switch name.  Although the cursor need not be at the beginning of the program argument, this
     *  method first matches a short name prefix at the beginning of the argument.  If the prefix ends at or before the cursor
     *  then the prefix must be immediately followed by a single-letter switch name, otherwise the prefix may be followed by
     *  one or more characters before a switch name is found exactly at the cursor.  In any case, if a name is found the cursor
     *  is advanced to the character after the name.  The returned name is the prefix and the character even if they don't
     *  occur next to each other in the program argument. */
    size_t matchShortName(Cursor&/*in,out*/, const ParsingProperties &props, std::string &name) const;

    /** @internal Explodes ListParser::ValueList elements of pvals, replacing pvals in place. */
    bool explode(ParsedValues &pvals /*in,out*/) const;

    /** Parse long switch arguments.  The the cursor is initialially positioned immediately after the switch name.  This
     *  matches the switch/value separator if necessary and subsequent switch arguments. On success, the cursor will be
     *  positioned at the beginning of the program argument following the last switch argument (or switch if no arguments) or
     *  at the end of the input.  Throws an <code>std::runtime_error</code> on failure without adjusting the cursor. */
    void matchLongArguments(const std::string &switchString, Cursor &cursor /*in,out*/, const ParsingProperties &props,
                            ParsedValues &result /*out*/) const;

    /** Parse short switch arguments.  The cursor is initially positioned immediately after the switch name.  On success, the
     *  cursor will be positioned at the first character after the last argument. This may be in the middle of a program
     *  argument or at the beginning of a program argument. */
    void matchShortArguments(const std::string &switchString, Cursor &cursor /*in,out*/, const ParsingProperties &props,
                             ParsedValues &result /*out*/, bool mayNestle) const;

    /** Parses switch arguments from the command-line arguments to obtain switch values.  Upon entry, the cursor should be
     *  positioned at the first character of the first argument. On success, for long switches the cursor will be positioned at
     *  the beginning of a subsequent program argument; for short switches it will either be positioned at the beginning of a
     *  subsequent program argument (like for long switches) or in the middle (neither beginning nor end) of a program argument
     *  at the character following the last argument.  The return value is the number of switch arguments parsed from the
     *  command-line and doesn't count switch arguments that came from argument default values. On failure, an
     *  <code>std::runtime_error</code> is thrown and the cursor is not adjusted. */
    size_t matchArguments(const std::string &switchString, const Location &endOfSwitch, Cursor &cursor /*in,out*/,
                          const ParsingProperties &props, ParsedValues &result /*out*/, bool isLongSwitch) const;

    /** @internal Return synopsis markup for a single argument. */
    std::string synopsisForArgument(const SwitchArgument&) const;

    // If the synopsis is empty, create one from an optional switch group and name space separator
    std::string synopsis(const ParsingProperties &swProps, const SwitchGroup *sg, const std::string &nameSpaceSeparator) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Switch groups
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** A collection of related switch declarations.
 *
 *  A switch group is an important concept in that it allows different software components to declare their own command lines,
 *  and those declarations can be used by some higher layer. The higher layer builds a parser from the various switch groups in
 *  order to simultaneously parse switches for all the software components.
 *
 *  In order to help avoid conflicting switches when inserting these parts (switch groups) into a parser, each switch group can
 *  have a name. A switch group's name is an optional string that can be inserted at the beginning of long switches to
 *  disambiguate them from switches with the same name in other groups.  For instance, if groups with the names "output" and
 *  "input" both have a long switch "--tab-width" then the user will (by default) get an error if they specify "--tab-width" on
 *  the command-line. The error message will indicate they should use either "--output-tab-width" or "--input-tab-width". The
 *  documentation, typically generated by "--help", describes the switches like "--[output-]tab-width", but authors may
 *  override the switch synopsis with their own string. Group names are only used to disambiguate long switches, not
 *  single-letter switches. A switch group name is different than the switch group title or dockey.
 *
 *  Tools that are unable to simultaneously parse on behalf of multiple software components (presumably because they're not
 *  using this library), sometimes run into problems where the argument for a switch which is not part of the current parsing
 *  language looks like a valid switch in the current language.  Consider a tool that uses single-hyphen prefixes for its long
 *  switches and has a switch name "go" that takes an argument, and lets say that this tool must also accept all the switches
 *  for the find(1) command but it doesn't actually know them--it just passes along all command line arguments it doesn't
 *  recognize to the find-like software component.  If the command line contained <code>-perm -go=w -print</code> then the tool
 *  will extract the <code>-go=w</code> switch that it recognizes, and pass along <code>-perm -print</code> to the other
 *  component component. Unfortunately, the <code>-go=w</code> that it recognized as its own was actually the argument for the
 *  <code>-perm</code> switch that it didn't recognize.
 *
 *  When creating a switch group, switch declarations (@ref Switch) are copied into the switch group.  Eventually the switch
 *  group itself is copied into a parser. */
class SAWYER_EXPORT SwitchGroup {
#include <Sawyer/WarningsOff.h>
    std::vector<Switch> switches_;
    ParsingProperties properties_;
    std::string name_;                                  // name optionally prepended to all switches
    std::string title_;                                 // title showing in documentation
    std::string docKey_;                                // for sorting
    std::string documentation_;
    SortOrder switchOrder_;
#include <Sawyer/WarningsRestore.h>
public:
    /** Construct an unnamed, untitled group. */
    SwitchGroup(): switchOrder_(DOCKEY_ORDER) { initializeLibrary(); }

    /** Construct a titled group.
     *
     *  Titling a group prevents its switches from being globally sorted with other groups when the documentation is produced.
     *  The @p title will appear as the title of a subsection for the switches and should be capitalized like a title (initial
     *  capital letters). The optional @p docKey is used to sort the groups in relation to each other (the default is to sort
     *  by group title). */
    explicit SwitchGroup(const std::string &title, const std::string &docKey="")
        : title_(title), docKey_(docKey), switchOrder_(DOCKEY_ORDER) {}

    /** Property: Title of the switch group.
     *
     *  A switch group may have a subsection title for documentation.  The title should be capitalized like a title.  The title
     *  may also be specified in the constructor.
     * @{ */
    const std::string& title() const { return title_; }
    SwitchGroup& title(const std::string &title) { title_ = title; return *this; }
    /** @} */

    /** Property: Group name.
     *
     *  A switch group may have a name in order to disambiguate command-line switches that match more than one switch
     *  declaration. This is useful if a user is constructing a parser from switch groups over which they have no control. For
     *  example, if two groups have a "--foo" switch then the user can give one group a name of "alpha" and the other "beta" in
     *  which case "--alpha:foo" matches one and "--beta:foo" matches the other.
     *
     *  Names can only resolve long switches, not short switches.
     *
     * @{ */
    const std::string& name() const { return name_; }
    SwitchGroup& name(const std::string &name) { name_ = name; return *this; }
    /** @} */

    /** Property: Documentation sort key.
     *
     *  This key is used to order the switch groups with respect to one another in the documentation.  Switches that belong to
     *  groups having the same documentation key are treated as if they came from the same group for the purpose of sorting the
     *  switches within groups.  Any switch group that has no documentation key will use the lower-case group title (which may
     *  also be the empty string).  If more than one switch group has the same documentation key but different titles, then
     *  only one of those titles is arbitrarily chosen as the subsection title in the documentation.
     *
     *  The documentation key may also be specified in the constructor.
     *
     * @sa SwitchGroup::switchOrder
     * @{ */
    const std::string& docKey() const { return docKey_; }
    SwitchGroup& docKey(const std::string &key) { docKey_ = key; return *this; }
    /** @} */

    /** Property: Detailed description.
     *
     *  This is the description of the switch group in a simple markup language. See @ref Switch::doc for a description of the
     *  markup language.  Documentation for a switch group will appear prior to the switches within that group.  If multiple
     *  groups are to appear in the same section of the manual page (by virtue of having the same @ref SwitchGroup::title then
     *  their documentation strings are appended as separate paragraphs in the order that the switch groups appear in the
     *  parser.
     *
     *  Documentation specified by this property is in addition to automatically generated documentation for the switches
     *  within this group.
     *
     * @{ */
    SwitchGroup& doc(const std::string &s) { documentation_ = s; return *this; }
    const std::string& doc() const { return documentation_; }
    /** @} */

    /** Property: How to show group name in switch synopsis.
     *
     *  A switch group can override how the group name appears in a switch synopsis. The default is to inherit the setting from
     *  the parser containing the group at the time of documentation.
     *
     * @{ */
    ShowGroupName showingGroupNames() const { return properties_.showGroupName; }
    void showingGroupNames(ShowGroupName x) { properties_.showGroupName = x; }
    /** @} */

    /** @copydoc Switch::resetLongPrefixes
     * @{ */
    SwitchGroup& resetLongPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                   const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    SwitchGroup& longPrefix(const std::string &s1) { properties_.longPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& longPrefixes() const { return properties_.longPrefixes; }
    /** @} */

    /** @copydoc Switch::resetShortPrefixes
     * @{ */
    SwitchGroup& resetShortPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                    const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    SwitchGroup& shortPrefix(const std::string &s1) { properties_.shortPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& shortPrefixes() const { return properties_.shortPrefixes; }
    /** @} */

    /** @copydoc Switch::resetValueSeparators
     * @{ */
    SwitchGroup& resetValueSeparators(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                      const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    SwitchGroup& valueSeparator(const std::string &s1) { properties_.valueSeparators.push_back(s1); return *this; }
    const std::vector<std::string>& valueSeparators() const { return properties_.valueSeparators; }
    /** @} */

    /** Number of switches declared. */
    size_t nSwitches() const { return switches_.size(); }

    /** List of all declared switches. The return value contains the switch declarations in the order they were inserted into
     *  this switch group. */
    const std::vector<Switch>& switches() const { return switches_; }

    /** Returns true if a switch with the specified name exists.  Both long and short names are checked. */
    bool nameExists(const std::string &switchName);

    /** Returns the first switch with the specified name. Both long and short names are checked. Throws an exception if no
     *  switch with that name exists. */
    const Switch& getByName(const std::string &switchName);

    /** Returns true if a switch with the specified key exists. */
    bool keyExists(const std::string &switchKey);

    /** Returns the first switch with the specified key.  Throws an exception if no switch exists having that key. */
    const Switch& getByKey(const std::string &switchKey);

    /** Insert a switch into the group.  The switch declaration is copied into the group. */
    SwitchGroup& insert(const Switch&);

    /** Insert switches from another group into this one.  Other properties are not copied. */
    SwitchGroup& insert(const SwitchGroup&);

    /** Remove a switch from the group. The <em>n</em>th switch is removed, as returned by @ref switches. */
    SwitchGroup& removeByIndex(size_t n);

    /** Remove a switch from the group.  The first declaration with the specified name is erased from this group. Both long and
     *  short names are checked. */
    SwitchGroup& removeByName(const std::string &switchName);

    /** Remove a switch from the group.  The first declaration with the specified key is erased from this group. */
    SwitchGroup& removeByKey(const std::string &switchKey);

    /** Property: Order of switches in documentation.  This property controls the sorting of keys within the group.  If the
     *  property's value is @ref DOCKEY_ORDER, the default, then switches are sorted according to the Switch::docKey values; if
     *  the property is @ref INSERTION_ORDER then switch documentation keys are ignored and switches are presented in the order
     *  they were added to the group.
     *
     *  Since documentation will combine into a single subsection all the switches from groups having the same title, it is
     *  possible that the subsection will have conflicting orderings.  When this happens, the last group to be inserted is
     *  the one whose value is used for the entire subsection.
     *
     * @sa SwitchGroup::docKey
     * @{ */
    const SortOrder& switchOrder() const { return switchOrder_; }
    SwitchGroup& switchOrder(SortOrder order) { switchOrder_ = order; return *this; }
    /** @} */

public:
    // Used internally
    const ParsingProperties& properties() const { return properties_; }

private:
    friend class Parser;
    bool removeByPointer(const void*);
};

/** Subset of switches grouped by their switch groups. */
typedef Container::Map<const SwitchGroup*, std::set<const Switch*> > GroupedSwitches;

/** Subset of switches indexed by their command-line representation. */
typedef Container::Map<std::string, GroupedSwitches> NamedSwitches;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** The parser for a program command line.
 *
 *  A parser is configured to describe the valid program switches, their arguments, and other information, and then the parser
 *  is then applied to a program command line to return a ParserResult. The process of parsing a command line is free of
 *  side-effects other than creating the result. */
class SAWYER_EXPORT Parser {
#include <Sawyer/WarningsOff.h>
    std::vector<SwitchGroup> switchGroups_;             /**< Declarations for all recognized switches. */
    ParsingProperties properties_;                      /**< Some properties inherited by switch groups and switches. */
    std::string groupNameSeparator_;                    /**< String that separates group name from switch name. */
    std::vector<std::string> terminationSwitches_;      /**< Special switch to terminate parsing; default is "-\-". */
    bool shortMayNestle_;                               /**< Whether "-ab" is the same as "-a -b". */
    std::vector<std::string> inclusionPrefixes_;        /**< Prefixes that mark command line file inclusion (e.g., "@"). */
    bool skipNonSwitches_;                              /**< Whether to skip over non-switch arguments. */
    bool skipUnknownSwitches_;                          /**< Whether to skip over switches we don't recognize. */
    mutable std::string programName_;                   /**< Name of program, or "" to get (and cache) name from the OS. */
    std::string purpose_;                               /**< One-line program purpose for makewhatis (part after the "-"). */
    std::string versionString_;                         /**< Version string defaulting to "alpha". */
    mutable std::string dateString_;                    /**< Version date defaulting to current month and year. */
    int chapterNumber_;                                 /**< Standard Unix man page chapters 0 through 9. */
    std::string chapterName_;                           /**< Chapter name, or "" to use standard Unix chapter names. */
    typedef Container::Map<std::string, std::string> StringStringMap;
    StringStringMap sectionDoc_;                        /**< Extra documentation for any section by lower-case section name. */
    StringStringMap sectionOrder_;                      /**< Maps section keys to section names. */
    Message::SProxy errorStream_;                       /**< Send errors here and exit instead of throwing runtime_error. */
    Optional<std::string> exitMessage_;                 /**< Additional message before exit when errorStream_ is not empty. */
    SortOrder switchGroupOrder_;                        /**< Order of switch groups in the documentation. */
    bool reportingAmbiguities_;                         /**< Whether to report ambiguous switches. */
#include <Sawyer/WarningsRestore.h>

public:
    /** Default constructor.  The default constructor sets up a new parser with defaults suitable for the operating
     *  system. The switch declarations need to be added (via @ref with) before the parser is useful. */
    Parser()
        : groupNameSeparator_("-"), shortMayNestle_(true), skipNonSwitches_(false), skipUnknownSwitches_(false),
          versionString_("alpha"), chapterNumber_(1), chapterName_("User Commands"), switchGroupOrder_(INSERTION_ORDER),
          reportingAmbiguities_(true) {
        init();
    }

    /** Add switch declarations. The specified switch declaration or group of switch declarations is copied into the parser. A
     *  documentation key can be supplied to override the sort order for the group or switch.
     * @{ */
    Parser& with(const SwitchGroup &sg) {
        switchGroups_.push_back(sg);
        return *this;
    }
    Parser& with(const SwitchGroup &sg, const std::string &docKey) {
        switchGroups_.push_back(sg);
        switchGroups_.back().docKey(docKey);
        return *this;
    }
    Parser& with(const std::vector<SwitchGroup> &sgs) {
        switchGroups_.insert(switchGroups_.end(), sgs.begin(), sgs.end());
        return *this;
    }
    Parser& with(const Switch &sw) {
        switchGroups_.push_back(SwitchGroup().insert(sw));
        return *this;
    }
    Parser& with(Switch sw, const std::string &docKey) {
        sw.docKey(docKey);
        switchGroups_.push_back(SwitchGroup().insert(sw));
        return *this;
    }
    /** @} */

    /** List of all switch groups.
     *
     * @{ */
    const std::vector<SwitchGroup>& switchGroups() const {
        return switchGroups_;
    }
    std::vector<SwitchGroup>& switchGroups() {
        return switchGroups_;
    }
    /** @} */

    /** Predicate to determine whether a switch group exists.
     *
     *  Searches this parser for a switch group having the specified name and returns true if it exists. */
    bool switchGroupExists(const std::string &name) const;

    /** Switch group having specified name.
     *
     *  Searches this parser and returns the first switch group having the specified name. Throws a @ref
     *  Sawyer::Exception::NotFound error if no such @ref SwitchGroup exists in this parser.
     *
     * @{ */
    const SwitchGroup& switchGroup(const std::string &name) const;
    SwitchGroup& switchGroup(const std::string &name);
    /** @} */

    /** Remove switch group from parser.
     *
     *  Removes the first switch group having the specified name from this parser. Returns true if a switch group
     *  was removed, false if nothing was removed. */
    bool eraseSwitchGroup(const std::string &name);

    /** Property: Whether to report ambiguities.
     *
     *  If true, report ambiguous switches. Switches that can not be disambiguated are reported regardless of whether the
     *  switch is encountered on a commandline in order to help authors detect situations where it's impossible to control some
     *  setting from the command line.  Switches that can be disambiguated are reported only when they occur on the
     *  commandline, and include a message about how to disambiguate them.
     *
     *  If false, then no ambiguities are checked or reported. When encountering an ambiguous switch, the first matching
     *  definition is used.
     *
     * @{ */
    bool reportingAmbiguities() const { return reportingAmbiguities_; }
    Parser& reportingAmbiguities(bool b) { reportingAmbiguities_ = b; return *this; }
    /** @} */

    /** Property: String separating group name from switch name.
     *
     *  If switch group names are present, this property holds the string that separates the group name from the
     *  switch name.  For instance, if the group name is "group" and the switch name is "switch", the prefix is "--" and the
     *  separator is "-", then the switch can be parsed as either "--switch" or as "--group-switch".
     *
     * @{ */
    const std::string& groupNameSeparator() const { return groupNameSeparator_; }
    Parser& groupNameSeparator(const std::string &s) { groupNameSeparator_ = s; return *this; }
    /** @} */

    /** Property: How to show group names in switch documentation.
     *
     *  When generating a switch synopsis and the group containing the switch has a non-empty name, the group name can be added
     *  to the switch name.  The default is to add the group name as an optional part of the switch, like "--[group-]switch"
     *  since the parser treats them as optional unless the abbreviated name is ambiguous.
     *
     *  See also, @ref groupNameSeparator, @ref SwitchGroup::name.
     *
     * @{ */
    ShowGroupName showingGroupNames() const { return properties_.showGroupName; }
    Parser& showingGroupNames(ShowGroupName x) { properties_.showGroupName = x; return *this; }
    /** @} */

    /** Prefixes to use for long command-line switches.  The @ref resetLongPrefixes clears the list (and adds prefixes) while
     *  @ref longPrefix only adds another prefix to the list.  The default long switch prefix on Unix-like systems is
     *  <code>-\-</code>, but this can be overridden or augmented by switch groups and switch declarations.
     * @{ */
    Parser& resetLongPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                              const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Parser& longPrefix(const std::string &s1) { properties_.longPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& longPrefixes() const { return properties_.longPrefixes; }
    /** @} */

    /** Prefixes to use for short command-line switches.  The @ref resetShortPrefixes clears the list (and adds prefixes) while
     *  @ref shortPrefix only adds another prefix to the list.  The default short switch prefix on Unix-like systems is
     *  <code>-</code>, but this can be overridden or augmented by switch groups and switch declarations.
     * @{ */
    Parser& resetShortPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                               const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Parser& shortPrefix(const std::string &s1) { properties_.shortPrefixes.push_back(s1); return *this; }
    const std::vector<std::string>& shortPrefixes() const { return properties_.shortPrefixes; }
    /** @} */

    /** Strings that separate a long switch from its value.  The @ref resetValueSeparators clears the list (and adds
     *  separators) while @ref valueSeparator only adds another separator to the list.  The separator " " is special: it
     *  indicates that the argument for a switch must appear in a separate program argument (i.e., <code>-\-author matzke</code>
     *  as opposed to <code>-\-author=matzke</code>).  The default value separators on Unix-like systems are "=" and " ", but
     *  this can be overridden or augmented by switch groups and switch declarations.
     * @{ */
    Parser& resetValueSeparators(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Parser& valueSeparator(const std::string &s1) { properties_.valueSeparators.push_back(s1); return *this; }
    const std::vector<std::string>& valueSeparators() const { return properties_.valueSeparators; }
    /** @} */

    /** Strings that indicate the end of the argument list.  The @ref resetTerminationSwitches clears the list (and adds
     *  terminators) while @ref terminationSwitch only adds another terminator to the list.  The default terminator on
     *  Unix-like systems is <code>-\-</code>.
     * @{ */
    Parser& resetTerminationSwitches(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                   const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Parser& terminationSwitch(const std::string &s1) { terminationSwitches_.push_back(s1); return *this; }
    const std::vector<std::string>& terminationSwitches() const { return terminationSwitches_; }
    /** @} */

    /** Indicates whether short switches can nestle together.  If short switches are allowed to nestle, then <code>-ab</code>
     *  is the same as <code>-a -b</code> in two separate program arguments.  This even works if the short switch takes an
     *  argument as long as the argument parsing ends at the next short switch name.  For instance, if "a" takes an integer
     *  argument then <code>-a100b</code> will be parsed as <code>-a100 -b</code>, but if "a" takes a string argument the
     *  entire "100b" will be parsed as the value for the "a" switch.  The default on Unix-like systems is that short switches
     *  may nestle.
     * @{ */
    Parser& shortMayNestle(bool b) { shortMayNestle_ = b; return *this; }
    bool shortMayNestle() const { return shortMayNestle_; }
    /** @} */

    /** Strings that indicate that arguments are to be read from a file.  The @ref resetInclusionPrefixes clears the list (and
     *  adds prefixes) while @ref inclusionPrefix only adds another prefix to the list.  The default inclusion prefix on
     *  Unix-like systems is <code>\@</code>.  That is, a program argument <code>\@foo.conf</code> will be replaced with
     *  arguments read from the file "foo.conf". See @ref readArgsFromFile and @ref expandIncludedFiles for details.
     *
     *  For instance, to make file inclusion look like a normal switch,
     * @code
     *  Parser parser();
     *  parser.resetInclusionPrefixes("--file=");
     * @endcode
     * @{ */
    Parser& resetInclusionPrefixes(const std::string &s1=STR_NONE, const std::string &s2=STR_NONE,
                                   const std::string &s3=STR_NONE, const std::string &s4=STR_NONE);
    Parser& inclusionPrefix(const std::string &s1) { inclusionPrefixes_.push_back(s1); return *this; }
    const std::vector<std::string>& inclusionPrefixes() const { return inclusionPrefixes_; }
    /** @} */

    /** Whether to skip over non-switch arguments when parsing.  If false, parsing stops at the first non-switch, otherwise
     *  non-switches are simply skipped over and added to the parsing result that's eventually returned. In either case,
     *  parsing stops when a terminator switch (usually <code>-\-</code>) is found. Anything that looks like a switch but
     *  doesn't match a declaration continues to result in an error regardless of this property.
     *
     * @sa ParserResult::skippedArgs ParserResult::unparsedArgs
     * @{ */
    Parser& skippingNonSwitches(bool b) { skipNonSwitches_ = b; return *this; }
    bool skippingNonSwitches() const { return skipNonSwitches_; }
    /** @} */

    /** Whether to skip over unrecognized switches.  An unrecognized switch is any program argument that looks like a switch
     *  but which doesn't match the name of any declared switch.  When not skipping (the default) such program arguments throw
     *  an "unrecognized switch" <code>std::runtime_error</code>.
     *
     * @sa ParserResult::skippedArgs ParserResult::unparsedArgs
     * @{ */
    Parser& skippingUnknownSwitches(bool b) { skipUnknownSwitches_ = b; return *this; }
    bool skippingUnknownSwitches() const { return skipUnknownSwitches_; }
    /** @} */

    /** Specifies a message stream to which errors are sent.  If non-null, when a parse method encounters an error it writes
     *  the error message to this stream and exits.  The default, when null, is that errors cause an
     *  <code>std::runtime_error</code> to be thrown.  The various "skip" properties suppress certain kinds of errors entirely.
     *
     *  Note, Message::SProxy objects are intermediaries returned by the <code>[]</code> operator of Message::Facility, and
     *  users don't normally interact with them explicitly.  They're only present because c++11 <code>std::move</code>
     *  semantics aren't widely available yet.
     *
     *  For example, to cause command-line parsing errors to use the Sawyer-wide FATAL stream, say this:
     *
     * @code
     *  Parser parser;
     *  parser.errorStream(Message::mlog[Message::FATAL]);
     * @endcode
     *
     * @sa @ref skippingNonSwitches @ref skippingUnknownSwitches
     * @{ */
    Parser& errorStream(const Message::SProxy &stream) { errorStream_ = stream; return *this; }
    const Message::SProxy& errorStream() const { return errorStream_; }
    /** @} */

    /** Extra text to print before exit. This is only used when the @ref errorStream property is non-null.  The default is to
     *  emit the message "invoke with '-\-help' to see usage information." if a switch with the name "help" is present, or
     *  nothing otherwise.
     *
     * @sa @ref errorStream
     * @{ */
    Parser& exitMessage(const std::string &s) { exitMessage_ = s; return *this; }
    std::string exitMessage() const { return exitMessage_ ? *exitMessage_ : std::string(); }
    /** @} */

    /** Parse program arguments.  The first program argument, <code>argv[0]</code>, is considered to be the name of the program
     *  and is not parsed as a program argument.  This function does not require that <code>argv[argc]</code> be a member of
     *  the argv array (normally, <code>argv[argc]==NULL</code> in <code>main</code>). */
    ParserResult parse(int argc, char *argv[]);

    /** Parse program arguments.  The vector should be only the program arguments, not a program name or final empty string. */
    ParserResult parse(const std::vector<std::string>&);

#if 0 /* [Robb Matzke 2014-03-04] */
    /** Parse program arguments. The arguments are specified as iterators to strings and should be only program arguments, not
     *  a program name or final empty string. */
    template<typename Iterator>
    ParserResult parse(Iterator begin, Iterator end) {
        std::vector<std::string> args(begin, end);
        return parse(args);
    }
#endif

    /** Read a text file to obtain arguments.  The specified file is opened and each line is read to obtain a vector of
     *  arguments.  Blank lines and lines whose first non-space character is "#" are ignored.  The remaining lines are split
     *  into one or more arguments at white space.  Single and double quoted regions within a line are treated as single
     *  arguments (the quotes are removed).  The backslash can be used to escape quotes, white space, and backslash; any other
     *  use of the backslash is not special. */
    static std::vector<std::string> readArgsFromFile(const std::string &filename);

    /** Expand file arguments.
     *
     *  Scans the specified argument list looking for file inclusion switches and replacing those switches with the
     *  the file. */
    std::vector<std::string> expandIncludedFiles(const std::vector<std::string> &args);

    /** Bit flags for argument grouping. See @ref regroupArgs. */
    enum GroupingFlags {
        DEFAULT_GROUPING        = 0,                    /**< Zero, all flags are clear. */
        PROHIBIT_EMPTY_GROUPS   = 0x0001,               /**< Error if any group is empty. */
        SPLIT_SINGLE_GROUP      = 0x0002                /**< Split single group into singleton groups. */
    };

    /** Group arguments by "--" separators.
     *
     *  Given a vector of command-line arguments, regroup them into sub-vectors by using the special "--" arguments to separate
     *  the groups.  The @p flags is a bit vector that controls some of the finer aspects of grouping (see @ref
     *  GroupingFlags). The number of returned groups (after flags are processed) must fall within the specified @p limits. If
     *  any error is encountered then either print an error message and exit, or throw an <code>std::runtime_error</code>,
     *  depending on whether an @ref errorStream is defined. */
    std::vector<std::vector<std::string> >
    regroupArgs(const std::vector<std::string> &args,
                const Container::Interval<size_t> &limits = Container::Interval<size_t>::whole(),
                unsigned flags = 0 /*GroupingFlags*/);

    /** Program name for documentation.  If no program name is given (or it is set to the empty string) then the name is
     *  obtained from the operating system.
     * @{ */
    Parser& programName(const std::string& programName) { programName_ = programName; return *this; }
    const std::string& programName() const;
    /** @} */

    /** Program purpose.  This is a short, one-line description of the command that will appear in the "NAME" section of
     *  a Unix man page and picked up the the makewhatis(8) command.  The string specified here should be the part that
     *  appears after the hyphen, as in "foo - frobnicate the bar library".
     * @{ */
    Parser& purpose(const std::string &purpose) { purpose_ = purpose; return *this; }
    const std::string& purpose() const { return purpose_; }
    /** @} */

    /** Program version.  Every program should have a version string and a date of last change. If no version string is given
     *  then "alpha" is assumed; if no date is given then the current month and year are used.
     * @{ */
    Parser& version(const std::string &versionString, const std::string &dateString="");
    Parser& version(const std::pair<std::string, std::string> &p) { return version(p.first, p.second); }
    std::pair<std::string, std::string> version() const;
    /** @} */

    /** Manual chapter. Every Unix manual page belongs to a specific chapter.  The chapters are:
     *
     *  @li @e 1 -- User commands that may be started by everyone.
     *  @li @e 2 -- System calls, that is, functions provided by the kernel.
     *  @li @e 3 -- Subroutines, that is, library functions.
     *  @li @e 4 -- Devices, that is, special files in the /dev directory.
     *  @li @e 5 -- File format descriptions, e.g. /etc/passwd.
     *  @li @e 6 -- Games, self-explanatory.
     *  @li @e 7 -- Miscellaneous, e.g. macro packages, conventions.
     *  @li @e 8 -- System administration tools that only root can execute.
     *  @li @e 9 -- Another (Linux specific) place for kernel routine documentation.
     *
     *  Do not use chapters "n", "o", or "l" (in fact, only those listed integers are accepted).  If a name is supplied it
     *  overrides the default name of that chapter.  If no chapter is specified, "1" is assumed.
     * @{ */
    Parser& chapter(int chapterNumber, const std::string &chapterName="");
    Parser& chapter(const std::pair<int, std::string> &p) { return chapter(p.first, p.second); }
    std::pair<int, std::string> chapter() const;
    /** @} */

    /** Documentation for a section of the manual.  The user may define any number of sections with any names. Names should
     *  be capitalized like titles (initial capital letter), although case is insensitive in the table that stores them. The
     *  sections of a manual page are sorted according to lower-case versions of either the @p docKey or the @p sectionName. If
     *  a section's documentation is completely empty (no user specified documentation and no automatically generated
     *  documentation) then it will not show up in the output.
     *
     *  Some sections have content that's generatated automatically. For these sections, setting the doc string will either
     *  override the generated content or augment the content as described below.  Since setting the doc string to an empty
     *  string only suppresses any user-defined content and not the auto-generated content, this doesn't delete the section
     *  from the output. In order to delete the section, set its doc string to "delete" (this also works for sections that have
     *  no auto-generated content).
     *
     *  The following sections are always present in this order unless explicitly deleted by setting their doc string to
     *  "delete":
     *
     *  @li "Name" contains the program name and purpose separated from one another by a single hyphen. By convention, the
     *      purpose should be a short string with no capitalization (except special words) and no terminating punctuation.  If
     *      no user documentation is specified then this section is generated automatically from the parser's @ref purpose
     *      property.
     *
     *  @li "Synopsis" contains information about how to invoke the program.  If the user does not provide documentation, then
     *      an automatically generated value is used, which says to invoke the program by its name followed by zero or more
     *      switches.
     *
     *  @li "Description" is the detailed description of the program. It has no automatically generated content.
     *
     *  @li "Switches" lists all the non-hidden switches organized by @ref SwitchGroup.
     *
     *  @li All user-defined sections are inserted next.
     *
     *  @li "See Also" lists other man pages that were referenced prior to this point in the documentation.
     *
     *  @li "Documentation Issues" lists any non-fatal problems found in the documentation up to this point, such as
     *      switches that were referenced but not declared (e.g., misspelled).
     *
     *  The documentation is specified with a simple markup languge described by Switch::doc.
     *
     * @{ */
    Parser& doc(const std::string &sectionName, const std::string &docKey, const std::string &text);
    Parser& doc(const std::string &sectionName, const std::string &text) { return doc(sectionName, sectionName, text); }
    std::vector<std::string> docSections() const;
    std::string docForSwitches() const;
    std::string docForSection(const std::string &sectionName) const;
    /** @} */

    /** Full documentation.
     *
     *  Combines all the documentation parts to return a string documenting the entire parser.  The returned string contains
     *  markup in the Sawyer::Markup language, with some extensions specific to command-line parsing. */
    std::string documentationMarkup() const;

    /** Generate Perl POD documentation.
     *
     *  Generates a Perl POD string for this parser. */
    std::string podDocumentation() const;

    /** Generate plain text documentation. */
    std::string textDocumentation() const;

    /** Print documentation to standard output. Use a pager if possible. */
    void emitDocumentationToPager() const;

    template<class Grammar>
    void emitDocumentationToPager() const {
        Grammar grammar;
        initDocGrammar(grammar);
        grammar.title(programName(), boost::lexical_cast<std::string>(chapter().first), chapter().second);
        grammar.version(version().first, version().second);
        grammar.emit(documentationMarkup());
    }

    /** Property: How to order switch groups in documentation.  If the parser contains titled switch groups then switches will
     *  be organized into subsections based on the group titles, and this property controls how those subsections are ordered
     *  with respect to each other.  The subsections can be sorted according to the order they were inserted into the parser,
     *  or alphabetically by their documentation keys or titles.
     * @{ */
    SortOrder switchGroupOrder() const { return switchGroupOrder_; }
    Parser& switchGroupOrder(SortOrder order) { switchGroupOrder_ = order; return *this; }
    /** @} */

    /** Insert records for long switch strings.
     *
     *  Inserts records into the @p index for long switch strings depending on whether the switch string is canonical. */
    void insertLongSwitchStrings(Canonical, NamedSwitches &index /*in,out*/) const;

    /** Insert records for short switch strings.
     *
     *  Insert records into the @p index for short switch strings. */
    void insertShortSwitchStrings(NamedSwitches &index /*in,out*/) const;

    /** Insert records for long and short switch strings.
     *
     *  Insert records into the @p index for both long and short switch strings. This is just a convenient way to invoke @ref
     *  insertLongSwitchStrings and @ref insertShortSwitchStrings. */
    void insertSwitchStrings(Canonical, NamedSwitches &index /*in,out*/) const;

    /** Print a switch index.
     *
     *  This is mostly for debugging. It's quite easy to traverse the @ref NamedSwitches object and print them yourself. */
    static void printIndex(std::ostream&, const NamedSwitches&, const std::string &linePrefix = "");

    /** Find switch string ambiguities.
     *
     *  Return an index containing all switches that are ambiguous regardless of whether they're canonical. */
    NamedSwitches findAmbiguities() const;

    /** Find unresolvable switch string ambiguities.
     *
     *  Return an index containing all switches that are ambiguous and which cannot be made unambiguous by qualifying them. */
    NamedSwitches findUnresolvableAmbiguities() const;

    /** Remove the switch by matching parse sentence.
     *
     *  Removes from this parser whichever switch is able to parse the specified command-line. The input should be either a
     *  single command line argument string (like "--debug-level=5") or a vector of strings (like {"--debug-level", "5"}). Only
     *  long-name switches (not single-letter switches) can be removed this way.  Only the first matched switch is removed from
     *  the parser.
     *
     *  Returns either nothing, or a copy of the switch parser that was removed.  Since the switch parser is returned, it can
     *  then be modified and added back to the same parser or to a different parser.
     *
     * @{ */
    Sawyer::Optional<Switch> removeMatchingSwitch(const std::string &arg);
    Sawyer::Optional<Switch> removeMatchingSwitch(const std::vector<std::string> &args);
    /** @} */

public:
    // Used internally
    const ParsingProperties& properties() const { return properties_; }

private:
    void init();

    // Implementation for the public parse methods.
    ParserResult parseInternal(const std::vector<std::string> &programArguments);

    // Parse one switch from the current position in the command line and return the switch descriptor.  If the cursor is at
    // the end of the command line then return false without updating the cursor or parsed values.  If the cursor is at a
    // termination switch (e.g., "--") then consume the terminator and return false.  If the switch name is valid but the
    // arguments cannot be parsed, then throw an error.  If the cursor is at what appears to be a switch but no matching switch
    // declaration can be found, then throw an error.  The cursor will not be modified when an error is thrown.
    bool parseOneSwitch(Cursor&, const NamedSwitches &ambiguities, ParserResult&/*out*/);

    /** Parse one long switch.  Upon entry, the cursor should be positioned at the beginning of a program argument. On success,
     *  the cursor will be positioned at the beginning of a subsequent program argument, or at the end of input.  If a switch
     *  is successfully parsed, a pointer to the switch is returned and values are appended to @p parsedValues (the returned
     *  pointer is valid only as long as this parser is allocated). If no switch is available for parsing then the null pointer
     *  is returned. If some other parsing error occurs then a null value is returned and the @p saved_error is updated to
     *  reflect the nature of the error.  This function does not throw <code>std::runtime_error</code> exceptions. */
    const Switch* parseLongSwitch(Cursor&, ParsedValues&, const NamedSwitches &ambiguities, Optional<std::runtime_error>&);

    /** Parse one short switch.  Upon entry, the cursor is either at the beginning of a program argument, or at the beginning
     *  of a (potential) short switch name. On success, for non-nestled switches the cursor will be positioned at the beginning
     *  of a subsequent program argument, but for nestled switches it may be in the middle of a program argument (neither the
     *  beginning nor the end).  If a switch is successfully parsed, a pointer to the switch is returned and values are
     *  appended to @p parsedValues (the returned pointer is valid only as long as this parser is allocated). If no switch is
     *  available for parsing then the null pointer is returned. If some other parsing error occurs then a null value is
     *  returned and the @p saved_error is updated to reflect the nature of the error.  This function does not throw
     *  <code>std::runtime_error</code> exceptions. */
    const Switch* parseShortSwitch(Cursor&, ParsedValues&, const NamedSwitches &ambiguities,
                                   Optional<std::runtime_error>&, bool mayNestle);

    // Returns true if the program argument at the cursor looks like it might be a switch.  Apparent switches are any program
    // argument that starts with a long or short prefix.
    bool apparentSwitch(const Cursor&) const;

    // Returns the best prefix for each switch--the one used for documentation
    void preferredSwitchPrefixes(Container::Map<std::string, std::string> &prefixMap /*out*/) const;

    // Construct an error message for an ambiguous switch, switchString, having an optional group name part (including the
    // separator), and the required switch name.  The switchString must be present in the ambiguities table.
    std::string ambiguityErrorMesg(const std::string &longSwitchString, const std::string &optionalPart,
                                   const std::string &longSwitchName, const NamedSwitches &ambiguities);
    std::string ambiguityErrorMesg(const std::string &shortSwitchString, const NamedSwitches &ambiguities);

    // Initialize a documentation parser by registering things like @s, @man, etc.  The argument is a subclass such as
    // Document::PodMarkup.
    void initDocGrammar(Document::Markup::Grammar& /*in,out*/) const;

    // FIXME[Robb Matzke 2014-02-21]: Some way to parse command-lines from a config file, or to merge parsed command-lines with
    // a yaml config file, etc.
};

/** The result from parsing a command line.
 *
 *  The Parser::parse methods parse a command line without causing any side effects, placing all results in a ParserResult
 *  return value.  If parsing is successful, the user then queries the result (a "pull" paradigm) or applies the result (a
 *  "push" paradigm), or both.  In fact, even when the user only needs the pull paradigm for its own switches, it should still
 *  call @ref apply anyway so that other software layers whose switch groups may have been included in the parser will receive
 *  values for their program variables and have their actions called.
 *
 * @code
 *  SwitchGroup switches;
 *  Parser parser;
 *  parser.insert(switches);
 *  ParserResult cmdline = parser.parse(argc, argv);
 *  cmdline.apply();
 * @endcode
 *
 *  In fact, if the user doesn't need to do any querying (they use only the push paradigm), there's no reason he even needs to
 *  keep the parser result (or even the parser) in a variable:
 *
 * @code
 *  SwitchGroup switches;
 *  Parser().with(switches).parse(argc, argv).apply();
 * @endcode
 */
class SAWYER_EXPORT ParserResult {
#include <Sawyer/WarningsOff.h>
    Parser parser_;
    Cursor cursor_;
    ParsedValues values_;

    // Maps a name to indexes into the values_ vector.
    typedef Container::Map<std::string, std::vector<size_t> > NameIndex;
    NameIndex keyIndex_;                                // Values per switch key
    NameIndex switchIndex_;                             // Values per switch preferred name

    // List of parsed values organized by their location on the command line.  The location is for the switch itself even if
    // the values are spread out across subsequent argv members. We do it this way because many of the values are defaults that
    // don't actually have an argv location.  The integers are indexes into the values_ vector. In other words, this is a
    // mapping from switch location to values_ elements for the switch's values.
    typedef Container::Map<Location, std::vector<size_t> > ArgvIndex;
    ArgvIndex argvIndex_;

    // Information about program arguments that the parser skipped over. Indexes into argv_.
    typedef std::vector<size_t> SkippedIndex;
    SkippedIndex skippedIndex_;

    // Information about terminator switches like "--". Indexes into argv_.
    SkippedIndex terminators_;

    /** Switch actions to be called by @ref apply. We save one action per key. */
    Container::Map<std::string, SwitchAction::Ptr> actions_;
#include <Sawyer/WarningsRestore.h>

private:
    friend class Parser;
    ParserResult(const Parser &parser, const std::vector<std::string> &argv): parser_(parser), cursor_(argv) {}

public:
    ParserResult() {}

    /** Saves parsed values in switch-specified locations.  This method implements the @e push paradigm mentioned in the class
     *  documentation (see @ref ParserResult). */
    const ParserResult& apply() const;

    /** Returns the number of values for the specified key. This is the number of values actually stored for switches using
     * this key, which might be fewer than the number of values parsed.  See Switch::whichValue. */
    size_t have(const std::string &switchKey) const {
        return keyIndex_.getOrDefault(switchKey).size();
    }

    /** Returns values for a key.  This is the usual method for obtaining a value for a switch.  During parsing, the arguments
     *  of the switch are converted to @ref ParsedValue objects and stored according to the key of the switch that did the
     *  parsing.  For example, if <code>-\-verbose</code> has an intrinsic value of 1, and <code>-\-quiet</code> has a value of
     *  0, and both use a "verbosity" key to store their result, here's how one would obtain the value for the last occurrence
     *  of either of these switches:
     *
     * @code
     *  int verbosity = cmdline.parsed("verbosity").last().asInt();
     * @endcode
     *
     *  If it is known that the switches both had a Switch::whichValue property that was @ref SAVE_LAST then the more efficient
     *  version of @c parse with an index can be used:
     *
     * @code
     *  int verbosity = cmdline.parsed("verbosity", 0).asInt();
     * @endcode
     *
     * @{ */
    const ParsedValue& parsed(const std::string &switchKey, size_t idx) const;
    ParsedValues parsed(const std::string &switchKey) const;
    /** @} */

    /** Program arguments that were skipped over during parsing.
     *
     *  If the Parser::skippingUnknownSwitches or Parser::skippingNonSwitches properties are true, then this method returns
     *  those command-line arguments that the parser skipped.  The library makes no distinction between these two classes of
     *  skipping because in general, it is impossible to be accurate about it (see @ref SwitchGroup for an example).
     *
     *  Program arguments inserted into the command line due to file inclusion will be returned in place of the file inclusion
     *  switch itself.
     *
     * @sa unparsedArgs */
    std::vector<std::string> skippedArgs() const;

    /** Returns program arguments that were not reached during parsing.
     *
     *  These are the arguments left over when the parser stopped. Program arguments inserted into the command line due to file
     *  inclusion will be returned in place of the file inclusion switch itself.
     *
     * @sa unparsedArgs */
    std::vector<std::string> unreachedArgs() const;

    /** Returns unparsed switches.
     *
     *  Unparsed switches are those returned by @ref skippedArgs and @ref unreachedArgs.
     *
     *  The returned list includes termination switches (like <code>-\-</code>) if @p includeTerminators is true even if those
     *  switches were parsed. This can be useful when the parser is being used to remove recognized switches from a
     *  command-line.  If the original command line was <code>-\-theirs -\-mine -\- -\-other</code> and the parser recognizes
     *  only <code>-\-mine</code> and the <code>-\-</code> terminator, then the caller would probably want to pass
     *  <code>-\-theirs -\- -\-other</code> to the next software layer, which is exactly what this method returns when
     *  @p includeTerminators is true.  @b Beware: removing command-line arguments that are recognized by a parser that has an
     *  incomplete picture of the entire language is not wise--see @ref SwitchGroup for an example that fails.
     *
     *  Program arguments inserted into the command-line due to file inclusion will be returned in place of the file inclusion
     *  switch itself. */
    std::vector<std::string> unparsedArgs(bool includeTerminators=false) const;

    /** Returns the program arguments that were processed. This includes terminator switches that were parsed.
     *
     *  Program arguments inserted into the command-line due to file inclusion will be returned in place of the file inclusion
     *  switch itself. */
    std::vector<std::string> parsedArgs() const;

    /** The original command line.
     *
     *  This returns the original command line except that program arguments inserted into the command-line due to file
     *  inclusion will be returned in place of the file inclusion switch itself. */
    const std::vector<std::string>& allArgs() const { return cursor_.strings(); }

    /** That parser that created this result.  This is a copy of the parser that was used to create this result. */
    const Parser& parser() const { return parser_; }

private:
    // Insert more parsed values.  Values should be inserted one switch's worth at a time (or fewer)
    void insertValuesForSwitch(const ParsedValues&, const Parser*, const Switch*);
    void insertOneValue(const ParsedValue&, const Switch*, bool save=true);

    // Indicate that we're skipping over a program argument
    void skip(const Location&);

    // Add a terminator
    void terminator(const Location&);

    Cursor& cursor() { return cursor_; }
};

} // namespace
} // namespace

#endif
