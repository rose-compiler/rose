#ifndef Sawyer_Message_H
#define Sawyer_Message_H

#include "Sawyer.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <cstring>
#include <ctime>
#include <list>
#include <map>
#include <set>
#include <string>
#include <sys/time.h>
#include <vector>

// Define this if you don't have a c++-11 complaint library. If your library doesn't support std::move semantics for
// std::ostream then you must define this.  The semantics change slightly when this is defined: instead of log[INFO]
// returning a new stream each time, it returns a reference to an existing stream.  The biggest impact is that code
// written like this:
//
//     log[INFO] <<"the sum of 1 + 2 = " <<sum(1,2) <<"\n";
//
// might not work as expected if sum() also writes to log[INFO].  The logging system will see both functions writing
// to the same stream as if they were somehow coordinating to produce messages, some of which contain text from both
// functions.
//
// Defining SAWYER_MESSAGE_USE_PROXY also means that lazy use of streams is permitted. The following creates a single message
// when defined, but two messages (a canceled partial message and a complete message) when c++-11 move support is available:
//
//     log[INFO] <<"starting";
//     do_something();
//     log[INFO] <<" done.\n";
//
// The proper, non-lazy way write that (which also works if do_something writes to log[INFO]), is to save the "starting"
// message's stream so the message can be completed later:
//
//     SProxy m1(log[INFO].dup());
//     *m1 <<"starting";
//     do_something();
//     *m1 <<" done.\n";
//
// When c++-11 move support is available the stream proxy object "m1" will be replace with a Stream object and the dereference
// on the last line will become unecessary.
//
// Multi-thread support will not be implemented until c++-11 move semantics are more widely available.
#define SAWYER_MESSAGE_USE_PROXY

namespace Sawyer {

/** Event logging via messages.
 *
 *  This namespace provides functionality to conditionally emit diagnostic messages based on software component and message
 *  importance.
 *
 * @section intro Introduction
 *
 *  This system has the following features and design goals:
 *
 * <ul>
 *   <li>Familiarity. Sawyer uses C++ paradigms for output--code to emit a message looks like any code that writes to an STL
 *       output stream.</li>
 *   <li>Ease of use. Sawyer uses existing C++ output stream insertion operators, so no new code needs to be written to emit
 *       messages for objects.</li>
 *   <li>Type safety. Sawyer avoids the use of unsafe C printf-like varargs functions.</li>
 *   <li>Consistent output.  Sawyer messages have a consistent look since they all contain information about the program
 *       from whence they came.</li>
 *   <li>Partial messages. Sawyer is able to emit messages incrementally for things like progress bars.</li>
 *   <li>Readable output. Sawyer output is human readable even when partial messages overlap or messages are being generated
 *       from multiple threads.</li>
 *   <li>Granular logging. Sawyer supports multiple logging objects so each software component can have its own logging
 *       facility and coherently combines output across objects.</li>
 *   <li>Flexible output. Sawyer permits messages to be directed to one or more destinations, which can be an STL output
 *       stream, a C library FILE, a Unix file descriptor, the syslog daemon, or user-defined destinations. The user can also
 *       control how messages are formatted: information about the process, time stamps, colorized output, etc.</li>
 *   <li>Filtering and rate limiting.</li>
 *   <li>Run-time configurable. Sawyer has functions that allow collections of message streams to be controlled with a
 *       simple language that can be specified on command lines, in configuration files, from user input, etc.</li>
 * </ul>
 *
 *  Saywer messaging has two parts: low-level <em>plumbing</em> handles the transport of messages through a run-time
 *  constructed lattice to output destinations, and high-level <em>porcelain</em> is the part of the API with which most users
 *  interact.
 *
 *  The high-level porcelain's primary classes are, from bottom up:
 *
 *  <ul>
 *    <li><em>Stream</em> is a kind of std::ostream which creates messages via lines of text inserted with the <code><<</code>
 *        operator and sends them through the plumbing.</li>
 *    <li><em>Facility</em> is a collection of streams, one per importance level, all related to some particular software
 *        component.  For instance, in the ROSE Compiler project, the machine code disassembler might have its own Facility
 *        that distinct from the logging facility used by the assembly language unparser.  The importance levels are predefined
 *        as: debug, trace, where, info, warn, error, and fatal (see the Importance enum).</li>
 *    <li><em>Facilities</em> (note plural) is a collection of Facility objects that can be controlled as a group. One can
 *        think of a Facilities object as providing a name space for the Facility objects it contains. For instance, one
 *        can turn on all the debug-level messages except the disassembler messages by specifiying "debug,
 *        disassembler(!debug)" to the Facilities::control() method.</li>
 *  </ul>
 *
 * @section usage Basic message emission via porcelain
 *
 *  A useful usage pattern is to name all the Facility objects "logger" or "lg" ("log" is ambiguous with the logorithm function
 *  in  <cmath> if  namespaces are imported). Define a logger in the global scope and any successively smaller scopes to the
 *  extent desired.  To emit a message, write code like:
 *
 * @code
 *  #include <Sawyer/Message.h>
 *  using namespace Sawyer::Message;
 *  ...
 *  Message::log[DEBUG] <<"this is a debugging message.\n";
 * @endcode
 *
 *  The C++ compiler will resolve "log" to the most-locally available declaration, i.e., the most specific Facility. "[DEBUG]"
 *  selects the importance level, and the rest is normal C++ ostream idioms.  The insertion operators are all evaluated even
 *  when the stream is disabled, unless the "log[DEBUG]" is surrounded by the SAWYER_MESG() macro. This is a bit more typing,
 *  but is useful when the insertion operators are expensive:
 *
 * @code
 *  SAWYER_MESG(log[DEBUG]) <<"the result is " <<something_expensive() <<"\n";
 * @endcode
 *
 *  The SAWYER_MESG macro is just shorthand for evaluating <code>log[DEBUG]</code> in a boolean context and then
 *  short-circuiting the output expression:
 *
 * @code
 *  log[DEBUG] and log[DEBUG] <<"the result is " <<something_expensive() <<"\n";
 * @endcode
 *
 *  Obviously, one can also use "if" statements and conditional compilation if desired.
 *
 * @section facility Defining a message facility
 * 
 *  A Facility object, such as "log" in the previous examples is a collection of Stream objects, one per message importance
 *  level.  A user may create as many facilities as desired, and the recommended usage is to create one facility per software
 *  component: typically one global facility and a facility in each major namespace or class.  Each facility has a name as a
 *  C++ variable (often "log") and an optional string name. The string name appears (optionally) in the output and is also the
 *  name by which the facility is referenced when the facility is part of a collection of facilities in a Facilities
 *  object. The string name may contain "." and ":" as well as the usual C++ symbol naming syntax and is often the fully
 *  qualified name of the software component where it's defined.
 *
 *  By default, when a facility is created all the Stream objects it contains (one per importance level) will be connected to
 *  the same plumbing so that all the facility's messages go to the same place.  The plumbing is the second argument for the
 *  constructor (Sawyer::Message::merr is the default, which sends output to standard error).
 *
 * @code
 *  namespace Rose {
 *  namespace BinaryAnalysis {
 *  class Disassembler {
 *      static Sawyer::Message::Facility log;
 *  };
 *  Saywer::Message::Facility Disassembler::log("Rose::BinaryAnalsysis::Disassembler", Sawyer::Message::merr);
 * @endcode
 *
 * @section facilities Grouping and controlling facility objects
 *
 *  Any combination of Facility objects can be grouped together into one or more Facilities (note plural) objects so they can
 *  be enabled and disabled collectively. A Facilities object is able to parse a simple language that can be provided
 *  from the application's command-line, a configuration file, user input, hard-coded, or some other source. Sawyer is not
 *  itself a command-line switch parser--it only parses the strings which are provided to it.
 *
 *  Here's an example that creates three facilities and groups them into a single Facilities object:
 *
 * @code
 *  Facility mf1("main::mf1", merr);
 *  Facility mf2("main.mf2", merr);
 *  Facility mf3("message facility 3", merr);  //this name is used in the output, but...
 *  Facilities log_facilities;
 *  log_facilities.insert(mf1);
 *  log_facilities.insert(mf2);
 *  log_facilities.insert(mf3, "main.third");  //we need a valid name for the control language
 * @endcode
 *
 *  To enable output for all levels of importance except debugging, across all registered Facility objects, but only warning
 *  and higher messages for the mf1 Facility, you could do this:
 *
 * @code
 *  log_facilities.control("all, !debug, main::mf1(>=warn)")
 * @endcode
 *
 *  The terms are processed from left to right, but only if the entire string is valid. If the string is invalid then an
 *  exception is thrown to describe the error (including the exact position in the string where the error occurred).  The
 *  entire language is described in the documentation for Facilities::control().
 *
 * @section Plumbing
 * 
 *  The plumbing is a run-time constructed lattice containing internal nodes and leaf nodes. Each leaf node is a final
 *  destination for a message, such as output to a C++ ostream, a C FILE pointer, a Unix file descriptor, the syslog daemon,
 *  etc. The internal nodes serve as multiplexers, filters, rate limiters, etc.
 *
 *  All plumbing objects are dynamically allocated and reference counted. Instead of using the normal C++ constructors,
 *  plumbing objects are created with static "instance" methods that perform the allocation and return a smart pointer. The
 *  type names for smart pointers are the class names with a "Ptr" suffix.  We use Boost smart pointers (boost::shared_ptr) for
 *  the implementation.
 *
 *  For instance, a lattice that accepts any kind of message, limits the output to at most one message per second,
 *  and sends the messages to standard error and a file can be constructed like this:
 *
 * @code
 *  DestinationPtr d = TimeFilter::instance(1.0)
 *                       ->to(FdSink::instance(2),     //standard error on Unix
 *                            FileSink::instance(f));  // f is some FILE* we opened already
 * @endcode
 *
 *  The destination can then be used to construct a Stream or a Facility. When constructing a Facility, all its contained
 *  Stream objects will share the same destination (i.e., they will be collectively limited to one message per second).
 *
 * @code
 *  Facility log("sample", d);
 * @endcode
 *
 *  If you want the FATAL level to not be rate limited, you could adjust it like this:
 *
 * @code
 *  log[FATAL].destination(some_other_destination);
 *  //FIXME[Robb Matzke 2014-01-21]: This needs to be different when log[FATAL] returns
 *  // a copy of the stream.  Perhaps log.destination(FATAL, some_other_destination).
 * @endcode
 */
namespace Message {

/** Explicitly initialize the library. This initializes any global objects provided by the library to users.  This happens
 *  automatically for many API calls, but sometimes needs to be called explicitly. Calling this after the library has already
 *  been initialized does nothing. The function always returns true. */
bool initializeLibrary();

/** True if the library has been initialized. */
extern bool isInitialized;

/** Level of importance for a message. Higher values are generally of more importance than lower values. */
enum Importance {
    DEBUG,              /**< Messages intended to be useful primarily to the author of the code. This level of importance is
                         *   not often present in publically-released source code and serves mostly as an alternative for
                         *   authors that like to debug-by-print. */
    TRACE,             /**< Detailed tracing information useful to end-users that are trying to understand program
                         *   internals. These messages can also be thought of as debug messages that are useful to end
                         *   users. Tracing occurs in two levels, where TRACE is the low-level tracing that includes all
                         *   traceable messages. It can be assumed that if TRACE messages are enabled then WHERE messages are
                         *   also enabled to provide the broader context. */
    WHERE,             /**< Granular tracing information useful to end-users that are trying to understand program internals.
                         *   These can also be thought of as debug messages that are useful to end users.  Tracing occurs in
                         *   two levels, where WHERE provides a more granular overview of the trace. */
    INFO,               /**< Informative messages. These messages confer information that might be important but do not
                         *   indicate situations that are abnormal. */
    WARN,               /**< Warning messages that indicate an unusual situation from which the program was able to fully
                         *   recover. */
    ERROR,              /**< Error messages that indicate an abnormal situation from which the program was able to at least
                         *   partially recover. */
    FATAL,              /**< Messages that indicate an abnormal situation from which the program was unable to
                         *   recover. Producing a message of this type does not in itself terminate the program--it merely
                         *   indicates that the program is about to terminate. Since one software component's fatal error might
                         *   be a calling components's recoverable error, exceptions should generally be thrown.  About the
                         *   only time FATAL is used is when a logic assertion is about to fail, and even then it's usually
                         *   called from inside assert-like functions. */
    N_IMPORTANCE        /**< Number of distinct importance levels. */
};

/** Colors used by sinks that write to terminals. Note that most modern terminal emulators allow the user to specify the actual
 *  colors that are displayed for each of these, so the display color might not match the color name. */
enum AnsiColor {
    COLOR_BLACK         = 0,    // the values are important: they are the ANSI foreground and background color offsets
    COLOR_RED           = 1,
    COLOR_GREEN         = 2,
    COLOR_YELLOW        = 3,
    COLOR_BLUE          = 4,
    COLOR_MAGENTA       = 5,
    COLOR_CYAN          = 6,
    COLOR_WHITE         = 7,
    COLOR_DEFAULT       = 8
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string stringifyImportance(Importance);            /**< Convert an Importance enum to a string. */
std::string stringifyColor(AnsiColor);                  /**< Convert an AnsiColor enum to a string. */
double now();                                           /**< Return current time as floating point seconds since the epoch. */
double timeval_delta(const timeval &begin, const timeval &end); /**< Floating ponit difference between two time values. */
std::string escape(const std::string&);                 /**< Convert a string to its C representation. */



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Colors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** ANSI Color specification for text written to a terminal. */
struct ColorSpec {
    AnsiColor foreground;                               /**< Foreground color, or COLOR_DEFAULT. */
    AnsiColor background;                               /**< Background color, or COLOR_DEFAULT. */
    boost::tribool bold;                                /**< Use ANSI "bold" attribute? */

    /** Constructs an object with default foreground and background colors. */
    ColorSpec(): foreground(COLOR_DEFAULT), background(COLOR_DEFAULT), bold(false) {}

    /** Constructs an object that specifies only a foreground color. */
    explicit ColorSpec(AnsiColor fg): foreground(fg), background(COLOR_DEFAULT), bold(false) {}

    /** Constructs an object with fully-specified colors. */
    ColorSpec(AnsiColor fg, AnsiColor bg, bool bold): foreground(fg), background(bg), bold(bold) {}

    /** Returns true if this object is in its default-constructed state. */
    bool isDefault() const { return COLOR_DEFAULT==foreground && COLOR_DEFAULT==background && !bold; }
};

/** Colors to use for each message importance. */
class ColorSet {
    ColorSpec spec_[N_IMPORTANCE];
public:
    /** Return a color set that uses only default colors.  Warning, error, and fatal messages will use the bold attribute, but
     *  no colors are employed. */
    static ColorSet blackAndWhite();

    /** Return a color set that uses various foreground colors for the different message importance levels. */
    static ColorSet fullColor();

    /** Returns a reference to the color specification for a particular message importance.
     *  @{ */
    const ColorSpec& operator[](Importance imp) const { return spec_[imp]; }
    ColorSpec& operator[](Importance imp) { return spec_[imp]; }
    /** @} */
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Message Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Message properties.  Each message property is optional.  When a message is sent through the plumbing, each node of the
 *  plumbing lattice may provide default values for properties that are not set, or may override properties that are set. */
struct MesgProps {
    boost::optional<std::string> facilityName;          /**< The name of the logging facility that produced this message. */
    boost::optional<Importance> importance;             /**< Message importance level. */
    boost::tribool isBuffered;                          /**< Is output buffered and emitted on a per-message basis? */
    boost::optional<std::string> completionStr;         /**< String to append to the end of each complete message. */
    boost::optional<std::string> interruptionStr;       /**< String to append when a partial message is interrupted. */
    boost::optional<std::string> cancelationStr;        /**< String to append to a partial message when it is destroyed. */
    boost::optional<std::string> lineTermination;       /**< Line termination for completion, interruption, and cancelation. */
    boost::tribool useColor;                            /**< Use ANSI escape sequences to colorize output? */

    MesgProps(): isBuffered(boost::indeterminate), useColor(boost::indeterminate) {}

    /** Merge the specified properties into this object and return new properties.  Each property of the return value will be
     *  the value from this object, except when this object's property is missing, in which case the property value from the
     *  argument is used (which may also be missing). */
    MesgProps merge(const MesgProps&) const;

    /** Print the values for all properties.  This is used mainly for debugging. */
    void print(std::ostream&) const;
};

std::ostream& operator<<(std::ostream &o, const MesgProps &props);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Type defintions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer.  Sawyer uses reference-counting smart pointers for most objects in order to alleviate the user from
 *  having to keep track of which objects own which other objects and when it is safe to delete an object.  This is especially
 *  convenient for the plumbing part of Sawyer where objects are connected to each other in a lattice and may have multiple
 *  parents.
 *
 *  Sawyer uses the following conventions for objects that use smart pointers:
 *
 *  <ol>
 *    <li>The name of the smart pointer type is the name of the base class followed by "Ptr".  For instance, a smart
 *        pointer for the FileSink class is named FileSinkPtr.</li>
 *    <li>C++ constructors for such objects are protected to that users don't inadvertently create such an object on
 *        the stack and then try to use its address in a smart pointer situation.</li>
 *    <li>Users should use the "instance" class methods instead of constructors to instantiate an instance of such a class.
 *        These methods allocate a new object and return a smart pointer to that object.</li>
 *    <li>Sawyer uses boost::shared_ptr for its smart pointer implementation.</li>
 * </ol>
 * @{ */
typedef boost::shared_ptr<class Destination> DestinationPtr;
typedef boost::shared_ptr<class Multiplexer> MultiplexerPtr;
typedef boost::shared_ptr<class Filter> FilterPtr;
typedef boost::shared_ptr<class SequenceFilter> SequenceFilterPtr;
typedef boost::shared_ptr<class TimeFilter> TimeFilterPtr;
typedef boost::shared_ptr<class ImportanceFilter> ImportanceFilterPtr;
typedef boost::shared_ptr<class Gang> GangPtr;
typedef boost::shared_ptr<class Prefix> PrefixPtr;
typedef boost::shared_ptr<class UnformattedSink> UnformattedSinkPtr;
typedef boost::shared_ptr<class FdSink> FdSinkPtr;
typedef boost::shared_ptr<class FileSink> FileSinkPtr;
typedef boost::shared_ptr<class StreamSink> StreamSinkPtr;
typedef boost::shared_ptr<class SyslogSink> SyslogSinkPtr;
/** @} */

typedef std::pair<DestinationPtr, MesgProps> BakedDestination;
typedef std::vector<BakedDestination> BakedDestinations;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Messages
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Message.  A message consists of a string and can be in a partial, completed, or canceled state.  More text can be added to
 *  a message while it is in the partial state, but once the message is marked as completed or canceled the text becomes
 *  immutable.  Messages also have properties, which are fed into the plumbing lattice and adjusted as they traverse to the
 *  final destinations during a process called "baking". */
class Mesg {
    static unsigned nextId_;                            // class-wide unique ID numbers
    unsigned id_;                                       // unique message ID
    std::string text_;                                  // text of the message
    bool isComplete_;                                   // true when the message is complete
    bool isCanceled_;                                   // true when message is being canceled without completing
    MesgProps props_;                                   // message properties
public:

    /** Creates a new, partial message that is empty. */
    Mesg()
        : id_(nextId_++), isComplete_(false), isCanceled_(false) {}

    /** Creates a new, partial message that is empty.  Sets the facility name and importance properties as specified. */
    Mesg(const std::string &facilityName, Importance imp)
        : id_(nextId_++), isComplete_(false), isCanceled_(false) {
        props_.facilityName = facilityName;
        props_.importance = imp;
    }

    /** Creates a new, partial message that is empty. Initializes message properties as specified. */
    explicit Mesg(const MesgProps &props)
        : id_(nextId_++), isComplete_(false), isCanceled_(false), props_(props) {}

    /** Creates a new, completed message with the specified text. */
    explicit Mesg(const std::string &mesg)
        : id_(nextId_++), text_(mesg), isComplete_(true), isCanceled_(false) {}

    /** Creates a new, completed message with the specified text. Also sets the facility name and importance properties
     *  as specified. */
    Mesg(const std::string &facilityName, Importance imp, const std::string &mesg)
        : id_(nextId_++), text_(mesg), isComplete_(true), isCanceled_(false) {
        props_.facilityName = facilityName;
        props_.importance = imp;
    }

    /** Creates a new, completed message with the specified text.  Also sets the message properties as specified. */
    Mesg(const MesgProps &props, const std::string &mesg)
        : id_(nextId_++), text_(mesg), isComplete_(true), isCanceled_(false), props_(props) {}

    /** Return unique message ID.  Each message has an identification number that is unique within a process. */
    unsigned id() const { return id_; }

    /** Return the message text. */
    const std::string& text() const { return text_; }

    /** Returns true if the message has entered the completed state. Once a message is completed its text cannot be changed. */
    bool isComplete() const { return isComplete_; }

    /** Returns true if the message has entered the canceled state.  Messages typically only enter this state when they
     *  are being destroyed. */
    bool isCanceled() const { return isCanceled_; }

    /** Returns true if the message has no text. */
    bool isEmpty() const { return text_.empty(); }

    /** Returns a reference to message properties. Once a message's properties are "baked" by sending them through the
     *  plumbing lattice, changing the message properties has no effect on the text that is emitted by the message
     *  sinks. Message streams typically bake the message when text is first added.
     *  @{ */
    const MesgProps& properties() const { return props_; }
    MesgProps& properties() { return props_; }
    /** @} */

    /** Cause the message to enter the completed state. Once a message enters this state its text cannot be modified. */
    void complete() { isComplete_ = true; }

    /** Cause the message to enter the canceled state.  This typically happens automatically when a message is being
     *  destroyed. Once a message enters this state its text cannot be modified. */
    void cancel() { isCanceled_ = true; }

    /** Append additional text to the end of the message.  If the message is in the completed or canceled state then
     *  an std::runtime_error is thrown.
     *  @{ */
    void insert(const std::string&);
    void insert(char);
    /** @} */

    /** Send a message to its final destinations. */
    void post(const BakedDestinations&) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Plumbing lattice nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for all types of message destinations. This is the base class for all nodes in the plumbing lattice. */
class Destination: public boost::enable_shared_from_this<Destination> {
protected:
    MesgProps dflts_;                                   // default property values merged into each incoming message
    MesgProps overrides_;                               // overrides applied to incoming message
protected:
    Destination() {
        initializeLibrary();
    }
public:
    virtual ~Destination() {}

    /** Default values for message properties. Any message property that has no value coming from a higher layer in the
     * plumbing lattice will be set to the default value if one is defined.
     *  
     *  @{ */
    const MesgProps& defaultProperties() const { return dflts_; }
    MesgProps& defaultProperties() { return dflts_; }
    /** @} */

    /** Overriding message properties.  Any overriding property value that is defined will override the value coming from a
     *  higher layer in the plumbing lattice.
     *  @{ */
    const MesgProps& overrideProperties() const { return overrides_; }
    MesgProps& overrideProperties() { return overrides_; }
    /** @} */

    /** Bakes message properties according to the plumbing lattice.  The given message properties are applied to the plumbing
     *  lattice rooted at this Destination, adjusted according to the default and override properties at this node of the
     *  lattice and all lower nodes.  The property values at the bottom nodes of the lattice are appended to the
     *  BakedDestinations argument. */
    virtual void bakeDestinations(const MesgProps&, BakedDestinations&);

    /** Causes a message to be emitted. The @p bakedProperties argument is one of the values returned by the bakeDestinations()
     *  method. */
    virtual void post(const Mesg&, const MesgProps &bakedProperties) = 0;

    /** Merge properties of this lattice node into the specified properties.  Any property in @p props that is not set will be
     *  set to this node's default value for the property (if any).  Any override property defined for this node will override
     *  the value (or lack of value) in @p props.  The merged result is returned. */
    MesgProps mergeProperties(const MesgProps &props);
};

/** Sends incoming messages to multiple destinations.  This is the base class for all internal nodes of the plumbing
 *  lattice. */
class Multiplexer: public Destination {
    typedef std::list<DestinationPtr> Destinations;
    Destinations destinations_;
protected:
    Multiplexer() {}
public:
    /** Allocating constructor. */
    static MultiplexerPtr instance() { return MultiplexerPtr(new Multiplexer); }

    virtual void bakeDestinations(const MesgProps&, BakedDestinations&) /*override*/;
    virtual void post(const Mesg&, const MesgProps&);

    /** Adds a child node to this node of the lattice.  An std::runtime_error is thrown if the addition of this child would
     *  cause the lattice to become malformed by having a cycle. */
    void addDestination(const DestinationPtr&);

    /** Removes the specified child from this node of the lattice. See also, to(). */
    void removeDestination(const DestinationPtr&);

    /** Add a child nodes to this node of the lattice and returns this node.  It is often more convenient to call this
     *  method instead of addDestination().
     *  @{ */
    MultiplexerPtr to(const DestinationPtr&);           // more convenient form of addDestination()
    MultiplexerPtr to(const DestinationPtr&, const DestinationPtr&);
    MultiplexerPtr to(const DestinationPtr&, const DestinationPtr&, const DestinationPtr&);
    MultiplexerPtr to(const DestinationPtr&, const DestinationPtr&, const DestinationPtr&, const DestinationPtr&);
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Filters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for internal nodes that filter messages.  Filtering is applied when properties are baked, which should happen
 *  exactly once for each message, usually just before the message is posted for the first time. */
class Filter: public Multiplexer {
protected:
    Filter() {}
public:
    virtual void bakeDestinations(const MesgProps&, BakedDestinations&) /*override*/;

    /** Predicate determines when a message should be forwarded onward.  This method is called once from bakeDestinations(),
     *  and if it returns true then the baking is forwarded on to the child nodes in the plumbing lattice. If this method
     *  returns false then none of the descendents will see the properties (unless they can be reached by some other path),
     *  and the unreached leaf nodes will not be returned by bakeDestinations(). */
    virtual bool shouldForward(const MesgProps&) = 0;   // return true if message should be forwarded to children when baking

    /** Called once by bakeDestinations if shouldForward() returned true.  This method is called after the properties
     *  have been forwarded to the child nodes. */
    virtual void forwarded(const MesgProps&) = 0;       // message was forwarded to children
};

/** Filters messages based on how many messages have been seen.  The first <em>S</em> messages are skipped (not forwarded to
 *  children), the first of every <em>T</em>th message thereafter is forwarded, for a total of <em>U</em> messages forwarded. */
class SequenceFilter: public Filter {
    size_t nSkip_;                                      // skip initial messages posted to this sequencer
    size_t rate_;                                       // emit only 1/Nth of the messages (0 and 1 both mean every message)
    size_t limit_;                                      // emit at most this many messages (0 means infinite)
    size_t nPosted_;                                    // number of messages posted (including those suppressed)
protected:
    SequenceFilter(size_t nskip, size_t rate, size_t limit)
        : nSkip_(nskip), rate_(rate), limit_(limit), nPosted_(0) {}
public:
    /** Construct an instance. The node will skip the first @p nskip messages, then forward the first of every @p rate
     *  following messages, for a grand total of @p limit messages forwarded.  If @p rate is zero or one then every message
     *  after the first @p nskip is forwarded (up to the @p limit).  If @p limit is zero then there is no limit. */
    static SequenceFilterPtr instance(size_t nskip, size_t rate, size_t limit) {
        return SequenceFilterPtr(new SequenceFilter(nskip, rate, limit));
    }

    /** Number of initial messages to skip.
     *  @{ */
    size_t nSkip() const { return nSkip_; }
    void nSkip(size_t n) { nSkip_ = n; }
    /** @} */

    /** Rate of messages to emit after initial messages are skipped.  A rate of <em>N</em> means the first message of every
     *  group of <em>N</em> messages is forwarded to children nodes in the plumbing lattice.  A rate of zero means the same
     *  thing as a rate of one--every message is forwarded.
     *  @{ */
    size_t rate() const { return rate_; }
    void rate(size_t n) { rate_ = n; }
    /** @} */

    /** Limit the total number of messages forwarded.  A value of zero means no limit is in effect.
     *  @{ */
    size_t limit() const { return limit_; }
    void limit(size_t n) { limit_ = n; }
    /** @} */

    /** Number of messages processed.  This includes messages forwarded and messages not forwarded. */
    size_t nPosted() const { return nPosted_; }

    virtual bool shouldForward(const MesgProps&) /*override*/;
    virtual void forwarded(const MesgProps&) /*override*/ {}
};

/** Filters messages based on time.  Any message posted within some specified time of a previously forwarded message will
 *  not be forwarded to children nodes in the plumbing lattice. */
class TimeFilter: public Filter {
    double initialDelay_;                               // amount to delay before emitting the first message
    double minInterval_;                                // minimum time between messages
    struct timeval prevMessageTime_;                    // time previous message was emitted
    struct timeval lastBakeTime_;                       // time cached by shouldForward, used by forwarded
    size_t nPosted_;                                    // number of messages posted (including those suppressed)
protected:
    explicit TimeFilter(double minInterval)
        : initialDelay_(0.0), minInterval_(minInterval) {
        memset(&prevMessageTime_, 0, sizeof(timeval));
        memset(&lastBakeTime_, 0, sizeof(timeval));
    }
public:
    /** Allocating constructor. Creates an instance that limits forwarding of messages to at most one message every
     *  @p minInterval seconds. */
    static TimeFilterPtr instance(double minInterval) {
        return TimeFilterPtr(new TimeFilter(minInterval));
    }

    /** The minimum time between messages.
     *  @{ */
    double minInterval() const { return minInterval_; }
    void minInterval(double d);
    /** @} */

    /** Delay before the next message.
     *  @{ */
    double initialDelay() const { return initialDelay_; }
    void initialDelay(double d);
    /** @} */

    /** Number of messages processed.  This includes messages forwarded and messages not forwarded. */
    size_t nPosted() const { return nPosted_; }

    virtual bool shouldForward(const MesgProps&) /*override*/;
    virtual void forwarded(const MesgProps&) /*override*/;
};

/** Filters messages based on importance level. For instance, to disable all messages except fatal and error messages:
 *
 * @code
 *  DestinationPtr d = ImportanceFilter::instance()->enable(FATAL)->enable(ERROR);
 * @endcode
 */
class ImportanceFilter: public Filter {
    bool enabled_[N_IMPORTANCE];
protected:
    explicit ImportanceFilter(bool dflt) {
        memset(enabled_, dflt?0xff:0, sizeof enabled_);
    }
public:
    /** Allocating constructor.  Creates an instance that either allows or disallows all importance levels depending on
     *  whether @p dflt is true or false, respectively. */
    static ImportanceFilterPtr instance(bool dflt) {
        return ImportanceFilterPtr(new ImportanceFilter(dflt));
    }

    /** The enabled/disabled state for a message importance level.
     *  @{ */
    bool enabled(Importance imp) const { return enabled_[imp]; }
    void enabled(Importance imp, bool b) { enabled_[imp] = b; }
    /** @} */

    /** Enable an importance level.  Returns this node so that the method can be chained. */
    ImportanceFilterPtr enable(Importance);

    /** Disable an importance level. Returns this node so that the method can be chained. */
    ImportanceFilterPtr disable(Importance);

    virtual bool shouldForward(const MesgProps&) /*override*/;
    virtual void forwarded(const MesgProps&) /*override*/ {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Support for final destinations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
// Keeps track of how much of a partial message was already emitted.
class HighWater {
    boost::optional<unsigned> id_;                      // ID number of last message to be emitted, if any
    MesgProps props_;                                   // properties used for the last emission
    size_t ntext_;                                      // number of characters of the message we've seen already
public:
    HighWater(): ntext_(0) {}
    explicit HighWater(const Mesg &m, const MesgProps &p) { emitted(m, p); }
    void emitted(const Mesg&, const MesgProps&);        // make specified message the high water mark
    void clear() { *this = HighWater(); }               // reset to initial state
    bool isValid() const;                               // returns true if high water is defined
    unsigned id() const { return *id_; }                // exception unless isValid()
    const MesgProps& properties() const { return props_; }
    size_t ntext() const { return ntext_; }             // zero if !isValid()
};

// A Gang is used to coordinate output from two or more sinks.  This is normally used by sinks writing the tty, such as a sink
// writing to stdout and another writing to stderr -- they need to coordinate with each other if they're both going to the
// terminal.  A gang just keeps track of what message was most recently emitted.
class Gang: public HighWater {
    typedef std::map<int, GangPtr> GangMap;
    static GangMap gangs_;                              // Gangs indexed by file descriptor or other ID
    static const int TTY_GANG = -1;
protected:
    Gang() {}
public:
    static GangPtr instance() { return GangPtr(new Gang); }
    static GangPtr instanceForId(int id);               // return a Gang for the specified ID, creating a new one if necessary
    static GangPtr instanceForTty() { return instanceForId(TTY_GANG); }
    static void removeInstance(int id);                 // remove specified gang from global list
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Messages prefixes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information printed at the beginning of each free-format message. */
class Prefix {
    ColorSet colorSet_;                                 // colors to use if props.useColor is true
    boost::optional<std::string> programName_;          // name of program as it will be displayed (e.g., "a.out[12345]")
    bool showProgramName_;
    bool showThreadId_;
    boost::optional<timeval> startTime_;                // time at which program started
    bool showElapsedTime_;
    bool showFacilityName_;                             // should the facility name be displayed?
    bool showImportance_;                               // should the message importance be displayed?
    void initFromSystem();                              // initialize data from the operating system
protected:
    Prefix()
        : showProgramName_(true), showThreadId_(true), showElapsedTime_(true), showFacilityName_(true), showImportance_(true) {
        initFromSystem();
        colorSet_ = ColorSet::fullColor();
    }
public:
    virtual ~Prefix() {}

    /** Allocating constructor. Creates a new prefix instance and returns a pointer to it. The new object is initialized
     *  with information about the thread that created it, such as program name, thread ID, time of creation, etc. */
    static PrefixPtr instance() { return PrefixPtr(new Prefix); }

    /** Colors to use for the prefix if coloring is enabled.
     * @{ */
    const ColorSet& colorSet() const { return colorSet_; }
    ColorSet& colorSet() { return colorSet_; }
    /** @} */

    /** Program name. The default is the base name of the file that was executed.  An "lt-" prefix is removed from the base
     *  name since this is typically added by libtool and doesn't correspond to the name that the user executed.
     * @{ */
    const boost::optional<std::string>& programName() const { return programName_; }
    void programName(const std::string &s) { programName_ = s; }
    /** @} */

    /** Reset the program name from operating system information. */
    void setProgramName();

    /** Whether to show the program name in the message prefix area.
     * @{ */
    bool showProgramName() const { return showProgramName_; }
    void showProgramName(bool b) { showProgramName_ = b; }
    /** @} */

    /** Whether to show the thread ID in the message prefix area.
     * @{ */
    bool showThreadId() const { return showThreadId_; }
    void showThreadId(bool b) { showThreadId_ = b; }
    /** @} */

    /** The start time when emitting time deltas.
     * @{ */
    const boost::optional<timeval> startTime() const { return startTime_; }
    void startTime(timeval t) { startTime_ = t; }
    /** @} */

    /** Reset the start time from operating system information.  On some systems this will be the time at which the first
     *  prefix object was created rather than the time the operating system created the main process. */
    void setStartTime();                                // set program start time by querying the clock

    /** Whether to show time deltas.
     * @{ */
    bool showElapsedTime() const { return showElapsedTime_; }
    void showElapsedTime(bool b) { showElapsedTime_ = b; }
    /** @} */

    /** Whether to show the facilityName property when the property has a value.
     * @{ */
    bool showFacilityName() const { return showFacilityName_; }
    void showFacilityName(bool b) { showFacilityName_ = b; }
    /** @} */

    /** Whether to show the importance property when the property has a value.
     * @{ */
    bool showImportance() const { return showImportance_; }
    void showImportance(bool b) { showImportance_ = b; }
    /** @} */

    /** Return a prefix string. */
    virtual std::string toString(const Mesg&, const MesgProps&) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Final destinations (sinks)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for final destinations that are free-format. These are destinations that emit messages as strings. */
class UnformattedSink: public Destination {
    GangPtr gang_;
    PrefixPtr prefix_;
protected:
    UnformattedSink() {
        gang_ = Gang::instance();
        prefix_ = Prefix::instance();
        init();
    }
public:
    /** Sink gang. Message sinks use a gang to coordinate their output. For instance, two sinks that both write to the same
     *  file should share the same gang.  A shared gang can also be used for sinks that write to two different locations that
     *  both happen to be visible collectively, such as when writing to standard error and standard output when both are
     *  writing to the terminal or when both have been redirected to the same file.  Not all sinks support automatically
     *  choosing the correct gang (e.g., C++ std::ofstream doesn't have the necessary capabilities), so it must be chosen
     *  manually sometimes for the output to be coordinated.  See Gang::instanceForId() and Gang::instanceForTty().
     * @{ */
    const GangPtr& gang() const { return gang_; }
    void gang(const GangPtr &g) { gang_ = g; }          // cause this sink to be coordinated with others
    /** @} */

    /** Object to generate message prefixes.
     * @{ */
    const PrefixPtr& prefix() const { return prefix_; }
    void prefix(const PrefixPtr &p) { prefix_ = p; }
    /** @} */

    /** Support function for emitting a message.  This string terminates the previous partial message from the same gang. This
     *  method must be called exactly once during render() for its possible side effects, although the return value doesn't
     *  have to be used. */
    virtual std::string maybeTerminatePrior(const Mesg&, const MesgProps&);

    /** Support function for emitting a message. This string is the prefix generated by calling Prefix::toString() if a prefix
     *  is necessary.  This method must be called exactly once during render() for its possible side effects, although the
     *  return value doesn't have to be used. */
    virtual std::string maybePrefix(const Mesg&, const MesgProps&);

    /** Support function for emitting a message. This string is the message body, or at least the part of the body that hasn't
     *  been emitted yet.  This method must be called exactly once during render() for its possible side effects, although the
     *  return value doesn't have to be used. */
    virtual std::string maybeBody(const Mesg&, const MesgProps&);

    /** Support function for emitting a message.  This string is the message termination, interruption, or cancelation and the
     *  following line termination as necessary. This method must be called exactly once during render() for its possible side
     *  effects, although the return value doesn't have to be used. */
    virtual std::string maybeFinal(const Mesg&, const MesgProps&);

    /** Support function for emitting a message.  The return string is constructed by calling maybeTerminatePrior(),
     *  maybePrefix(), maybeBody(), and maybeFinal() one time each (because some of them have side effects). */
    virtual std::string render(const Mesg&, const MesgProps&);
private:
    void init();
};

/** Send free-format messages to a Unix file descriptor. */
class FdSink: public UnformattedSink {
    int fd_;                                            // file descriptor or -1
protected:
    explicit FdSink(int fd): fd_(fd) { init(); }
public:
    /** Allocating constructor.  Constructs a new message sink that sends messages to the specified Unix file descriptor. */
    static FdSinkPtr instance(int fd) {
        return FdSinkPtr(new FdSink(fd));
    }

    virtual void post(const Mesg&, const MesgProps&) /*override*/;
private:
    void init();
};

/** Send free-format messages to a C FILE pointer. */
class FileSink: public UnformattedSink {
    FILE *file_;
protected:
    explicit FileSink(FILE *f): file_(f) { init(); }
public:
    /** Allocating constructor.  Constructs a new message sink that sends messages to the specified C FILE pointer. */
    static FileSinkPtr instance(FILE *f) {
        return FileSinkPtr(new FileSink(f));
    }

    virtual void post(const Mesg&, const MesgProps&) /*override*/;
private:
    void init();
};

/** Send free-format messages to a C++ I/O stream. */
class StreamSink: public UnformattedSink {
    std::ostream &stream_;
protected:
    explicit StreamSink(std::ostream &stream): stream_(stream) {}
public:
    /** Allocating constructor.  Constructs a new message sink that sends messages to the specified C++ output stream. */
    static StreamSinkPtr instance(std::ostream &stream) {
        return StreamSinkPtr(new StreamSink(stream));
    }

    virtual void post(const Mesg&, const MesgProps&) /*override*/;
};

/** Sends messages to the syslog daemon. */
class SyslogSink: public Destination {
protected:
    SyslogSink(const char *ident, int option, int facility);
public:
    /** Allocating constructor.  Constructs a new message sink that sends messages to the syslog daemon.  The syslog API doesn't
     *  use a handle to refer to the syslog, nor does it specify what happens when openlog() is called more than once, or
     *  whether the ident string is copied.  Best practice is to use only constant strings as the ident argument. */
    static SyslogSinkPtr instance(const char *ident, int option, int facility) {
        return SyslogSinkPtr(new SyslogSink(ident, option, facility));
    }
    virtual void post(const Mesg&, const MesgProps&) /*override*/;
private:
    void init();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Message streams
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Stream;

// Only used internally; most of the API is in MessageStream
class StreamBuf: public std::streambuf {
    friend class Stream;
    bool enabled_;                                      // is this stream enabled?
    MesgProps dflt_props_;                              // default properties for new messages
    Mesg message_;                                      // current message, never in an isComplete() state
    DestinationPtr destination_;                        // where messages should be sent
    BakedDestinations baked_;                           // destinations baked at the start of each message
    bool isBaked_;                                      // true if baked_ is initialized
    bool anyUnbuffered_;                                // true if any baked destinations are unbuffered

protected:
    StreamBuf(): enabled_(true), isBaked_(false), anyUnbuffered_(false) {}
    ~StreamBuf() { cancelMessage(); }
    virtual std::streamsize xsputn(const char *s, std::streamsize &n) /*override*/;
    virtual int_type overflow(int_type c = traits_type::eof()) /*override*/;

private:
    void completeMessage();                             // complete and post message, then start a new one
    void cancelMessage();                               // cancel message if necessary
    void bake();                                        // bake the destinations
    void post();                                        // post message if necessary
};

#ifdef SAWYER_MESSAGE_USE_PROXY
// Used only until GCC supports a c++-11 compliant library with movable std::ostream.  Adjusts reference counts for a stream
// and deletes the stream when the reference count hits zero.
class SProxy {
    Stream *stream_;
public:
    SProxy(): stream_(NULL) {}
    SProxy(std::ostream*); /*implicit*/
    SProxy(std::ostream&); /*implicit*/
    SProxy(const SProxy&);
    SProxy& operator=(const SProxy&);
    ~SProxy() { reset(); }
    Stream& operator*() { return *stream_; }
    Stream* operator->() { return stream_; }
    Stream* get() const { return stream_; }
    operator bool() const;
    void reset();
};
#endif

/** Converts text to messages.  A message stream is a subclass of std::ostream and therefore allows all the usual stream
 *  insertion operators (<code><<</code>).  A stream converts each line of output text to a single message, creating the
 *  message with properties defined for the stream and sending the results to a specified destination. Streams typically impart
 *  a facility name and importance level to each message via the stream's properties. */
class Stream: public std::ostream {
#ifdef SAWYER_MESSAGE_USE_PROXY
    friend class SProxy;
#endif
    size_t nrefs_;                                      // used when we don't have std::move semantics
    StreamBuf *streambuf_;                              // each stream has its own
public:

    /** Construct a stream and initialize its name and importance properties. */
    Stream(const std::string facilityName, Importance imp, const DestinationPtr &destination)
        : std::ostream(new StreamBuf), nrefs_(0), streambuf_(NULL) {
        streambuf_ = dynamic_cast<StreamBuf*>(rdbuf());
        assert(streambuf_!=NULL);
        assert(destination!=NULL);
        streambuf_->dflt_props_.facilityName = facilityName;
        streambuf_->dflt_props_.importance = imp;
        streambuf_->destination_ = destination;
        streambuf_->message_.properties() = streambuf_->dflt_props_;
    }

    /** Construct a stream and initialize its properties as specified. */
    Stream(const MesgProps &props, const DestinationPtr &destination)
        : std::ostream(new StreamBuf), nrefs_(0), streambuf_(NULL) {
        streambuf_ = dynamic_cast<StreamBuf*>(rdbuf());
        assert(streambuf_!=NULL);
        assert(destination!=NULL);
        streambuf_->dflt_props_ = props;
        streambuf_->destination_ = destination;
        streambuf_->message_.properties() = streambuf_->dflt_props_;
    }

    /** Construct a new stream from an existing stream.  If @p other has a pending message then ownership of that message
     *  is moved to this new stream. */
    Stream(const Stream &other)
        : std::ostream(new StreamBuf), nrefs_(0), streambuf_(NULL) {
        initFrom(other);
    }

    /** Initialize this stream from another stream.  If @p other has a pending message then ownership of that message
     *  is moved to this stream. */
    Stream& operator=(const Stream &other) {
        initFrom(other);
        return *this;
    }

    // Same as Stream(const Stream&) but declared so we can do things like this:
    //   Stream m1(log[INFO] <<"first part of the message");
    //   m1 <<"; finalize message\n";
    Stream(const std::ostream &other_)
        : std::ostream(rdbuf()), nrefs_(0), streambuf_(NULL) {
        const Stream *other = dynamic_cast<const Stream*>(&other_);
        if (!other)
            throw "Sawyer::Message::Stream initializer is not a Sawyer::Message::Stream (only a std::ostream)";
        initFrom(*other);
    }

    // Same as operator=(const Stream&) but declared so we can do things like this:
    //   Stream m1 = log[INFO] <<"first part of the message");
    //   m1 <<"; finalize message\n";
    Stream& operator=(const std::ostream &other_) {
        const Stream *other = dynamic_cast<const Stream*>(&other_);
        if (!other)
            throw "Sawyer::Message::Stream initializer is not a Sawyer::Message::Stream (only a std::ostream)";
        initFrom(*other);
    }
    
    ~Stream() {
        assert(0==nrefs_);
        delete streambuf_;
    }

#ifdef SAWYER_MESSAGE_USE_PROXY
    // Used for partial messages when std::move is missing
    SProxy dup() const {
        return SProxy(new Stream(*this));
    }
#endif

protected:
    // Initiaize this stream from @p other.  This stream will get its own StreamBuf (if it doesn't have one already), and
    // any pending message from @p other will be moved (not copied) to this stream.
    void initFrom(const Stream &other) {
        assert(other.streambuf_!=NULL);
        streambuf_ = dynamic_cast<StreamBuf*>(rdbuf());
        if (!streambuf_) {
            streambuf_ = new StreamBuf;
            rdbuf(streambuf_);
        }

        // If this stream has a partial message then it needs to be canceled.  This also resets things associated with
        // the message, such as the baked properties.
        streambuf_->cancelMessage();

        // Copy some stuff from other.
        streambuf_->enabled_ = other.streambuf_->enabled_;
        streambuf_->dflt_props_ = other.streambuf_->dflt_props_;
        streambuf_->destination_ = other.streambuf_->destination_;

        // Swap message-related stuff in order to move ownership of the message to this.
        streambuf_->message_.properties() = streambuf_->dflt_props_;
        std::swap(streambuf_->message_, other.streambuf_->message_);
        std::swap(streambuf_->baked_, other.streambuf_->baked_);
        std::swap(streambuf_->isBaked_, other.streambuf_->isBaked_);
        std::swap(streambuf_->anyUnbuffered_, other.streambuf_->anyUnbuffered_);
    }

public:
    /** Returns true if a stream is enabled. */
    bool enabled() const { return streambuf_->enabled_; }

    /** Returns true if this stream is enabled.  This implicit conversion to bool can be used to conveniently avoid expensive
     *  insertion operations when a stream is disabled.  For example, if printing MemoryMap is an expensive operation then the
     *  logging can be written like this to avoid formatting memorymap when logging is disabled:
     * @code
     *  logger[DEBUG] and logger[DEBUG] <<"the memory map is:" <<memorymap <<"\n";
     * @endcode
     *
     * In fact, the SAWYER_MESG macro does exactly this and can be used instead:
     * @code
     *  SAWYER_MESG(logger[DEBUG]) <<"the memory map is: " <<memorymap <<"\n";
     * @endcode
     */
    operator bool() { return enabled(); }

    /** See Stream::bool() */
    #define SAWYER_MESG(message_stream) message_stream and message_stream

    /** Enable or disable a stream.  A disabled stream buffers the latest partial message and enabling the stream will cause
     * the entire accumulated message to be emitted--whether the partial message immediately appears on the output is up to
     * the message sinks.
     * @{ */
    void enable(bool b=true);
    void disable() { enable(false); }
    /** @} */

//    /** Indentation level.  A stream that is indented inserts additional white space between the message prefix and the message
//     *  text.  Although indentation is adjusted on a per-stream basis and the stream is where the indentation is applied, the
//     *  actual indentation level is a property of the MessageSink. Therefore, all messages streams using the same sink are
//     *  affected.
//     *
//     *  Users seldom use this method to change the indentation since it requires careful programming to restore the old
//     *  indentation in the presence of non-local exits from scopes where the indentation was indented to be in effect (e.g.,
//     *  exceptions). A more useful interface is the MessageIndenter class.
//     * @{ */
//    size_t indentation() const { return streambuf_.sink_->indentation(); }
//    void indentation(size_t level) { streambuf_.sink_->indentation(level); }
//    /** @} */

    /** Set message or stream properties. If @p asDefault is false then the property is set only in the current message and
     *  the message must be empty (e.g., just after the previous message was completed).  When @p asDefault is true the new
     *  value becomes the default for all future messages. The default also affects the current message if the current message
     *  is empty.
     *  @{ */
    void completionString(const std::string &s, bool asDefault=true);
    void interruptionString(const std::string &s, bool asDefault=true);
    void cancelationString(const std::string &s, bool asDefault=true);
    void facilityName(const std::string &s, bool asDefault=true);
     /** @} */

    /** Accessor for the message destination.
     *  @{ */
    const DestinationPtr& destination() const {
        return streambuf_->destination_;
    }
    void destination(const DestinationPtr &d) {
        assert(d!=NULL);
        streambuf_->destination_ = d;
    }
    /** @} */

    /** Return the default properties for this stream. These are the properties of the stream itself before they are adjusted
     *  by the destination lattice. The default properties are used each time the stream creates a message. */
    MesgProps properties() const {
        return streambuf_->dflt_props_;
    }
};

/** Collection of streams. This forms a collection of message streams for a software component and contains one stream per
 *  message importance level.  A Facility is intended to be used by a software component at whatever granularity is desired by
 *  the author (program, name space, class, method, etc.) and is usually given a string name that is related to the software
 *  component which it serves.  The string name becomes part of messages and is also the default name used by
 *  Facilities::control().  All Stream objects created for the facility are given the same name, message prefix generator, and
 *  message sink, but they can be adjusted later on a per-stream basis.
 *
 *  The C++ name for the facility is often just "logger" (appropriately scoped) so that code to emit messages is self
 *  documenting. The name "log" is also often used, but is ambiguous with the log() function defined in "math.h".
 * @code
 *  logger[ERROR] <<"I got an error\n";
 * @endcode
 *
 *  When a Facility is intended to serve an entire program, the name of the facility is redundant with the program name printed
 *  by the default message prefix.  The best way to remedy this is to supply an empty name for the facility, and if the
 *  facility is registered with a Facilities object to supply the program name when it is registered:
 *
 * @code
 *  using namespace Sawyer;
 *  Facility logger("");
 *  Facilities facilities;
 *
 *  int main(int argc, char *argv[]) {
 *      std::string progname = get_program_name(argv[0]);
 *      facilities.insert(logger, progname);
 *      ...
 * @endcode
 */
class Facility {
    std::string name_;
    std::vector<SProxy> streams_;
public:
    /** Construct an empty facility.  The facility will have no name and all streams will be uninitialized.  Any attempt to
     *  emit anything to a facility in the default state will cause an std::runtime_error to be thrown with a message similar
     *  to "stream INFO is not initialized yet".  This facility can be initialized by assigning it a value from another
     *  initialized facility. */
    Facility() {}

    /** Create a named facility with default destinations.  All streams are enabled and all output goes to file descriptor
     *  2 (standard error) via unbuffered system calls.  Facilities initialized to this state can typically be used before the
     *  C++ runtime is fully initialized and before Sawyer::initializeLibrary() is called. */
    explicit Facility(const std::string &name): name_(name) {
        //initializeLibrary() //delay until later
        initStreams(FdSink::instance(2));
    }

    /** Creates streams of all importance levels. */
    Facility(const std::string &name, const DestinationPtr &destination): name_(name) {
        initializeLibrary();
        initStreams(destination);
    }

    /** Returns a stream for the specified importance level.  Returns a copy so that we can do things like this:
     *     Stream m1 = (log[INFO] <<"message 1 part 1");
     *     Stream m2 = (log[INFO] <<"message 2 part 1");
     *     m1 <<" part 2\n";
     *     m2 <<" part 2\n";
     * @{ */
#ifdef SAWYER_MESSAGE_USE_PROXY
    Stream& get(Importance imp);
    Stream& operator[](Importance imp) {
        return get(imp);
    }
#else
    // g++ 4.8 does not yet have have a standard-conforming library, so this doesn't work
    // clang 3.5 (trunk 198621) library conforms and works
    // Microsoft's implemention is reportedly conforming as of 2014-01-20
    // See http://www.cplusplus.com/forum/general/121964
    // The reason we want to return a new stream is so that we can add multi-threading support and this will work:
    //    log[DEBUG] <<"long message output by thread 1\n";
    //    log[DEBUG] <<"long message output by thread 2\n";
    // The two threads end up writing to different streams so that they create independent messages. If these functions
    // returned references instead of copies then the two threads are writing to the same stream, and their concurrent output
    // is being used to create a single sequence of messages. An individual message might have some text from one thread and
    // other text from another thread.
    Stream get(Importance imp) {
        assert((size_t)imp<streams_.size());
        Stream retval(*streams_[imp]);
        return std::move(retval)
    }
    Stream operator[](Importance imp) {
        assert((size_t)imp<streams_.size());
        Stream retval(*streams_[imp]);
        return std::move(retval)
    }
#endif
    /** @} */

    /** Return the name of the facility. This is a read-only field initialized at construction time. */
    const std::string name() const { return name_; }

    /** Cause all streams to use the specified destination.  This can be called for facilities that already have streams and
     *  destinations, but it can also be called to initialize the streams for a default-constructed facility. */
    void initStreams(const DestinationPtr&);
};

/** Collection of facilities.
 *
 *  A Facilities collection of Facility objects allows its members to be configured collectively, such as from command-line
 *  parsing.  Each stream can be enabled/disabled individually, a member Facility can be enabled/disabled as a whole, or
 *  specific importance levels can be enabled/disabled across the entire collection.
 *
 *  Whenever a Facility object is enabled as a whole, the current set of enabled importance levels is used.  This set is
 *  maintained automatically: when the first Facility object is inserted, its enabled importance levels initialize the default
 *  set.  Whenever a message importance level is enabled or diabled via the version of enable() or disable() that take an
 *  Importance argument, the set is adjusted. */
class Facilities {
public:
    typedef std::set<Importance> ImportanceSet;
private:
    typedef std::map<std::string, Facility*> FacilityMap;
    FacilityMap facilities_;
    ImportanceSet impset_;
    bool impsetInitialized_;
public:

    /** Constructs an empty container of Facility objects. */
    Facilities() {
        initializeLibrary();
        // Initialize impset_ for the sake of insertAndAdjust(), but do not consider it to be truly initialized until after
        // a MessageFacility is inserted.
        impset_.insert(WARN);
        impset_.insert(ERROR);
        impset_.insert(FATAL);
        impsetInitialized_ = false;
    }

    /** Return the set of default-enabled message importances. See class documentation for details. */
    const ImportanceSet& impset();

    /** Add or remove a default importance level. The specified level is inserted or removed from the set of default enabled
     *  importance levels without affecting any member Facility objects.  Calling this function also prevents the first
     *  insert() from initializing the set of default importance levels. See class documentation for details. */
    void impset(Importance, bool enabled);

    /** Register a Facility so it can be controlled as part of a collection of facilities.  The optional @p name is the
     *  FACILITY_NAME string of the control language for the inserted @p facility, and defaults to the facility's name.  The
     *  name consists of one or more symbols separated by '::', or '.' and often corresponds to the source code component that
     *  it serves.  Names are case-sensitive in the control language.  No two facilities in the same Facilities object can have
     *  the same name, but a single Facility may appear multiple times with different names.
     *
     *  This method comes in two flavors: insert(), and insertAndAdjust().  The latter immediately enables and disables the
     *  facility's streams according to the current default importance levels of this Facilities object.  If @p facility is the
     *  first Facility to be inserted, and it is inserted by insert() rather than insertAndAdjust(), then the facility's
     *  currently enabled streams are used to initialize the set of default enabled importance levels.
     *
     *  The @p facility is incorporated by reference and should not be destroyed until after this Facilities object is
     *  destroyed.
     * @{ */
    void insert(Facility &facility, std::string name="");
    void insertAndAdjust(Facility &facility, std::string name="");
    /** @} */

    /** Remove a facility by name. */
    void erase(const std::string &name) {
        facilities_.erase(name);
    }

    /** Remove all occurrences of the facility. */
    void erase(Facility &facility);

    /** Parse a single command-line switch and enable/disable the indicated streams.  Returns an empty string on success, or an
     *  error message on failure.  No configuration changes are made if a failure occurs.
     *
     *  The control string, @p s, is a sentence in the following language:
     *
     * @code
     *  Sentence             := (FacilitiesControl ( ',' FacilitiesControl )* )?
     *
     *  FacilitiesControl    := AllFacilitiesControl | NamedFacilityControl
     *  AllFacilitiesControl := StreamControlList
     *  NamedFacilityControl := FACILITY_NAME '(' StreamControlList ')'
     *
     *  StreamControlList    := StreamControl ( ',' StreamControl )*
     *  StreamControl        := StreamState? Relation? STREAM_NAME | 'all' | 'none'
     *  StreamState          := '+' | '!'
     *  Relation             := '>=' | '>' | '<=' | '<'
     * @endcode
     *
     *  The language is processed left-to-right and stream states are affected immediately.  This allows, for instance, turning
     *  off all streams and then selectively enabling streams.
     *
     *  Some examples:
     * @code
     *  all                             // enable all event messages
     *  none,debug                      // turn on only debugging messages
     *  all,!debug                      // turn on all except debugging
     *  fatal                           // make sure fatal errors are enabled (without changing others)
     *  none,<=trace                    // disable all except those less than or equal to 'trace'
     *
     *  frontend(debug)                 // enable debugging streams in the 'frontend' facility
     *  warn,frontend(!warn)            // turn on warning messages everywhere except the frontend
     * @endcode
     *
     *  The global list (AllFacilitiesControl) also affects the list of default-enabled importance levels.
     */
    std::string control(const std::string &s);

    /** Readjust all member facilities.  All members are readjusted to enable only those importance levels that are part of
     * this Facilities object's default importance levels.  It is as if we called disable() then enable() for each message
     * facility by name. See impset() and class documentation for details. */
    void reenable();
    void reenableFrom(const Facilities &other);

    /** Enable/disable specific importance level across all facilities.  This method also affects the set of "current
     *  importance levels" used when enabling an entire facility.
     * @{ */
    void enable(Importance, bool b=true);
    void disable(Importance imp) { enable(imp, false); }
    /** @} */

    /** Enable/disable a facility by name. When disabling, all importance levels of the specified facility are disabled. When
     *  enabling, only the current importance levels are enabled for the facility.  If the facility is not found then nothing
     *  happens.
     * @{ */
    void disable(const std::string &switch_name) { enable(switch_name, false); }
    void enable(const std::string &switch_name, bool b=true);
    /** @} */

    /** Enable/disable all facilities.  When disabling, all importance levels of all facilities are disabled.  When enabling,
     *  only the current importance levels are enabled for each facility. */
    void enable(bool b=true);
    void disable() { enable(false); }

    /** Print the list of facilities and their states. This is mostly for debugging purposes. The output may have internal
     *  line feeds and will end with a line feed. */
    void print(std::ostream&) const;

private:
    // Private info used by control() to indicate what should be adjusted.
    struct ControlTerm {
        ControlTerm(const std::string &facilityName, bool enable)
            : facilityName(facilityName), lo(DEBUG), hi(DEBUG), enable(enable) {}
        std::string toString() const;                   // string representation of this struct for debugging
        std::string facilityName;                       // optional facility name; empty implies all facilities
        Importance lo, hi;                              // inclusive range of importances
        bool enable;                                    // new state
    };

    // Error information thrown internally
    struct ControlError {
        ControlError(const std::string &mesg, const char *position): mesg(mesg), inputPosition(position) {}
        std::string mesg;
        const char *inputPosition;
    };

    // Functions used by the control() method
    static std::string parseFacilityName(const char* &input);
    static std::string parseEnablement(const char* &input);
    static std::string parseRelation(const char* &input);
    static std::string parseImportanceName(const char* &input);
    static Importance importanceFromString(const std::string&);
    static std::list<ControlTerm> parseImportanceList(const std::string &facilityName, const char* &input);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern DestinationPtr merr;                             // unbuffered output to standard error
extern Facility log;                                    // global logging facility
extern Facilities facilities;                           // global facilities object provided by Sawyer

} // namespace
} // namespace

#endif
