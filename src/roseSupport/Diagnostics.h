// Stuff related to ROSE diagnostics
#ifndef ROSE_Diagnostics_H
#define ROSE_Diagnostics_H

#include "Sawyer/Assert.h"
#include "Sawyer/Message.h"

// How failed assertions behave
#define ROSE_ASSERTION_ABORT    1                       // call abort()
#define ROSE_ASSERTION_EXIT     2                       // call exit(1)
#define ROSE_ASSERTION_THROW    3                       // throw rose::Diagnostics::FailedAssertion

namespace rose {

/** Controls diagnostic messages from ROSE.
 *
 *  ROSE uses the free [Sawyer](http://github.com/matzke1/sawyer) library to emit diagnostic messages for events such as
 *  debugging, program tracing, informational messages, warning and error messages, progress reports, and runtime logic
 *  assertions. The Sawyer message support can be found in the Sawyer::Message name space in "Sawyer/Message.h", and the
 *  run-time logic assertions can be found in the Sawyer::Assert name space in "assert/Assert.h" (which also defines a number
 *  of C preprocessor macros whose names begin with "ASSERT_".
 *
 *  Sawyer supports multiple instances of messaging facilities (Sawyer::Message::Facility or rose::Diagnostics::Facility) each
 *  of which defines a std::ostream object for each of a number of message importance levels.  ROSE defines one global
 *  library-wide facility, <code>rose::Diagnostics::mlog</code> whose string name (used in output) is simply "rose".  Software
 *  layers within ROSE may define their own facilities and give them names indicative of the software layer.  All these
 *  facilities are then grouped together into a single Sawyer::Message::Facilities object, rose::Diagnostics::mfacilities, so
 *  they can be controlled collectively or individually from the ROSE command-line (e.g., the frontend() call) via "-rose:log"
 *  switch.
 *
 * @section usage Command-line usage
 *
 *  ROSE looks for the command-line switch "-rose:log <em>WHAT</em>".  If <em>WHAT</em> is the word "help" then usage
 *  information is displayed; if <em>WHAT</em> is the word "list" then log settings are displayed.  Otherwise, <em>WHAT</em> is
 *  expected to be a string to pass to the rose::Diagnostics::mfacilities.control() function.  In short, the string is a
 *  comma-separated list of importance levels to enable (or disable when preceded by "!").  Importance levels can also be
 *  enclosed in parentheses and preceded by a facility name to restrict the settings to the specified facility.  For instance,
 *  if one wanted to turn off INFO messages for all facilities, and then turn on TRACE and DEBUG messages for the BinaryLoader,
 *  he would use "-rose:log '!info, BinaryLoader(trace, debug)'".  The single quotes are to prevent the shell from doing what
 *  it normally does for exclamation marks, spaces, and other punctuation.  See the doxygen documentation for
 *  Sawyer::Message::Facilities::control() for details, or use "-rose:log help".  Tools that don't call ROSE's frontend()
 *  usually use "--log" and "-L" to control logging. The "-rose:log" switch (or alternative) may appear multiple times on the
 *  command-line, and they are processed in the order they appear.
 *
 * @section adding Adding a facility to ROSE
 *
 *  Note: this section is about adding a new logging facility to the ROSE library proper. Adding a facility to a tool that uses
 *  ROSE is slightly different and is described in a later section.
 *
 *  As an example, let's say that a programmer wants to convert the BinaryLoader class to use its own logging facilities.  The
 *  first step is to declare a static data member for the facility. Adding a logging facility to a namespace or file is similar
 *  except the @c mlog would be declared in the namespace or as a file-scope (static or anonymous namespace) variable.
 *
 * @code
 *  // in BinaryLoader.h
 *  #include "Diagnostics.h"   // rose::Diagnostics
 *
 *  class BinaryLoader {
 *      ...
 *  protected:
 *      static Sawyer::Message::Facility mlog;
 *  };
 * @endcode
 *
 *  If we consistently name message facilities "mlog" then any unqualified reference to @c mlog in the source code will
 *  probably be the most appropriate (most narrowly scoped) facility.  Adding a new facility to a software component that
 *  already uses the rose-wide "mlog" is mostly a matter of defining a more narrowly scoped @c mlog, and the existing code will
 *  start using it (you probably also need to search for places where @c mlog usage was qualified). Specifically, we avoid
 *  using "log" as the name of any logging facility because it can conflict on some systems with the logorithm function @c log.
 *
 *  The second step is to define the @c mlog static variable. Since the variable is statically allocated, it must be
 *  constructed only with the default constructor. Sawyer also has other Facility constructors, but these generally can't be
 *  used for static variables because there is no portable way to ensure that the C++ runtime is initialized before the
 *  Facility initialization happens.  If the initializations occur in the wrong order then bizarre behavior may result,
 *  including segmentation faults in STL container classes.
 *
 * @code
 *  // in BinaryLoader.C
 *  #include "BinaryLoader.h"
 *  Sawyer::Message::Facility BinaryLoader::mlog;
 * @endcode
 *
 *  The third step is to initialize the @c mlog static variable after we're sure that the C++ runtime has been initialized. We
 *  can do this by defining a class method (a.k.a., static function member) that can be called from ROSE's initialization
 *  phase.  This is the point at which the mlog is given a name and connected to a destination(s).  The name is usually the
 *  fully qualified name of the component for which the @c mlog object serves as the logging facility, and may contain typical
 *  C++ symbol names, dots, and "::".  This name is used to enable/disable the facility from the command-line, and will show up
 *  as part of the output for any message emitted using this facility.
 *
 *  The call to Sawyer::Message::Facilities::insertAndAdjust adds this @c mlog facility to the global list of facilities so it
 *  can be controlled from the command line. It also immediately enables/disables the Facility Stream objects according to the
 *  settings in @c mfacilities.  Facilities are still usable if they're not registered in the global list -- they just can't be
 *  controlled by the user in the typical way.
 *
 * @code
 *  // class method (static member function) in BinaryLoader.C
 *  void BinaryLoader::initDiagnostics() {
 *      static bool initialized = false;
 *      if (!initialized) {
 *          initialized = true;
 *          mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::BinaryLoader", Diagnostics::destination);
 *          Diagnostics::mfacilities.insertAndAdjust(mlog);
 *      }
 *  }
 * @endcode
 *
 *  If the class, namespace, or file containing you @c initDiagnostics function is conditionally compiled (e.g., binary
 *  analysis is not always enabled in ROSE), you'll want to add a dummy version of @c initDiagnostics that does nothing
 *  (preferrable to using conditional compilation in the next step).
 *
 *  The fourth and final step is to add a call to BinaryLoader::initDiagnostics() from Diagnostics::initialize(). This function
 *  is defined in "src/roseSupport/Diagnostics.C". You probably don't need to include your entire header file in Diagnostics.C;
 *  a declarations should be sufficient and faster to compile.
 *
 * @section usage Using a facility in the ROSE library or tools
 *
 *  When using a message facility within the ROSE library source code, it is generally desirable to have a "using namespace
 *  rose::Diagnostics" in effect.  Not only does this alleviate the need to qualify the important levels (e.g.,
 *  <code>INFO</code>, <code>WARN</code>, etc), but it also brings rose::Diagnostics::mlog, the library-wide logging facility,
 *  into scope.  Doing so will allow any software component to perform logging using the library-wide facility, and once a more
 *  local @c mlog is declared the more local symbol is automatically used.
 *
 *  Generally speaking, all one needs to do to use a facility is to write a C++ std::ostream-style output statement whose
 *  left-hand operand is a Sawyer::Message::Stream.  Since streams are held in a Sawyer::Message::Facility and since facilities
 *  usually have the C++ name "mlog", the statement will look something like this:
 *
 * @code
 *  using rose::Diagnostics;
 *  mlog[INFO] <<"loading \"" <<filename <<"\"\n";
 * @endcode
 *
 *  The default configuration will cause the message to be emitted on the standard error stream along with information about
 *  the name of the executable, the thread ID, the time in seconds relative to the start of the process, the name of the
 *  facility, and the message importance.  For example:
 *
 * @code
 *  identityTranslator[30603] 0.00949s BinaryLoader[INFO]: loading "/usr/lib32/libc.so"
 * @endcode
 *
 *  ROSE also defines a global macro @c mprintf that is a drop-in replacement for @c printf.  This macro uses whatever
 *  <code>mlog[DEBUG]</code> is in scope.  The @ref mfprintf function can be used as a replacement when a specific logging
 *  stream is desired.
 *
 *  Sometimes one wants the right hand side of the output statements to be evaluated only in certain circumstances for
 *  performance reasons, and there are multiple ways to do that.  If you already use conditional compilation or "if" statements
 *  to generate diagnostic output, continue to do the same.  You can also use an "if" statement whose condition is the
 *  diagnostic stream since streams evaluated in a Boolean context return true or false depending on whether the stream is
 *  enabled or disabled:
 *
 * @code
 *  if (mlog[INFO])
 *      mlog[INFO] <<"loading \"" <<filename <<"\"\n";
 * @endcode
 *
 *  Sawyer also has a SAWYER_MESG() macro that's used like this:
 *
 * @code
 *  SAWER_MESG(mlog[INFO]) <<"loading \"" <<filename <<"\"\n";
 * @endcode
 *
 *  Another thing you can do is construct a new locally-declared stream with a shorter name.  Some parts of the ROSE library do
 *  things like this:
 *
 * @code
 *  using namespace rose::Diagnostics;
 *  void doSomething() {
 *      Stream debug(mlog[DEBUG]); // get our own copy of a stream
 *      ...
 *      if (debug) {
 *          some_debugging_things();
 *          debug <<"debugging results...\n";
 *      }
 *  }
 * @endcode
 *
 *  Not only does this reduce typing a little, but since the function is using it's own private message stream, partial
 *  messages emitted to that stream won't interfere with partial messages emitted to <code>mlog[DEBUG]</code> by called
 *  functions (see next section).
 *
 *  Another debugging trick is to create a local stream like above, and then explicitly enable or disable it based on some
 *  condition you're trying to debug:
 *
 * @code
 *  using namespace rose::Diagnostics;
 *  void doSomething(int foo) {
 *      Stream debug(mlog[DEBUG]); // get our own copy of a stream
 *      debug.enable(5==foo); // enable it only in certain cases
 *      ...
 *      if (debug) {
 *          some_debugging_things();
 *          debug <<"debugging results...\n";
 *      }
 *  }
 * @endcode
 *
 * @section partial Partial messages
 *
 *  A partial message is any message for which the linefeed, "\n", hasn't been inserted.  If a message stream is unbuffered and
 *  another message is emitted to a different stream and ends up going to the same ultimate destination (e.g., standard error),
 *  then any previous partial message is interrupted, usually by appending "...", and re-emitted later when more text is added.
 *
 *  Since each message stream creates messages sequentially in an incremental manner, we sometimes want to "save" a partial
 *  message, allow other messages to be emitted from the same stream, and then complete the original message later.  This most
 *  often happens when one function creates a partial message, and then calls another function that might use the same stream
 *  before the partial message is completed.  The best way to accomplish this is by creating a new, temporary message stream
 *  and moving any partial messages to the new stream.  The copy constructor can be used, like the example above, but even more
 *  conveniently, if the source stream in the copy constructor has an outstanding partial message then that partial message is
 *  moved (not copied) to the newly constructed stream.  This allows a partial message to be emitted in the same statement that
 *  constructs the local stream:
 *
 * @code
 *  Stream m1(mlog[INFO] <<"loading \"" <<filename <<"\""); //note no "\n"
 *  do_other_stuff_that_might_emit_info_messages();
 *  m1 <<"; done loading.\n"; //original message completed now
 *  // you may continue to use m1 for additional messages...
 * @endcode
 *
 *  The documentation for Sawyer::Message has additional hints and examples.
 *
 * @section tool Usage in Tools
 *
 *  Tools that are built on top of the ROSE library can use the same Sawyer messaging support, and the tool's Facility objects
 *  can be registered with ROSE and thus controlled along with the library's facilities.  Doing so is a three-step process:
 *  declare the facility in the tool, initialize it, and register the facility with ROSE.
 *
 *  The facility is usually declared as a static object so it can be available to all parts of the tool.  As with static
 *  facilities in the ROSE library itself (described above), it should be only default constructed.  Also, instead of using
 *  namespace @ref rose::Diagnostics, the tool uses namespace @ref Sawyer::Message::Common.  This avoids the ambiguity for @c
 *  mlog that would be created by importing @ref rose::Diagnostics or @ref Sawyer::Message.
 *
 * @code
 *  #include <rose/Diagnostics.h>
 *  using namespace Sawyer::Message::Common;
 *  Sawyer::Message::Facility mlog;
 * @endcode
 *
 *  The other two steps, initialization and registration, are usually near the beginning of main():
 *
 * @code
 *  rose::Diagnostics::initialize();
 *  mlog = Sawyer::Message::Facility("toolName", rose::Diagnostics::destination);
 *  rose::Diagnostics::mfacilities.insertAndAdjust(mlog);
 * @endcode
 *
 *  The first line ensures that the global variables in rose::Diagnostics are properly initialized. You don't need to do this
 *  if you've already initialized the ROSE library in some other manner.
 *
 *  The assignment initializes our logging facility by giving it a name and connecting it to the same message destination(s)
 *  used by the ROSE library (probably standard error).
 *
 *  The call to @c insertAndAdjust registers the tool's logging facility with the ROSE library and enables and disables the
 *  tool's message streams so they're in the same state as the ROSE library streams (probably debug and tracing messages are
 *  disabled and info, warn, error, and fatal are enabled).
 *
 *  The tool can parse arguments in it's normal way.  If ROSE's frontend() is called then the "-rose:log HOW" switch is
 *  processed, otherwise we recommend that the tool uses "--log" and "-L" switches to control logging. @ref Sawyer::CommandLine
 *  has methods for automatically parsing and applying these switches, or the tool can parse its own command-line and pass the
 *  argument to the @ref rose::Diagnostics::mfacilities.control method.
 */
namespace Diagnostics {

// Make Sawyer message importance levels available here. That way, we don't have to "use namespace Sawyer::Message", which
// also sucks in a lot of other stuff.
using Sawyer::Message::DEBUG;                           // very low-level debugging mostly for developers
using Sawyer::Message::TRACE;                           // more low-level debugging but maybe more interesting to users
using Sawyer::Message::WHERE;                           // higher-level debugging to help users understand ROSE flow
using Sawyer::Message::MARCH;                           // progress bars and other high-frequency messages
using Sawyer::Message::INFO;                            // useful information about normal behavior such as stats
using Sawyer::Message::WARN;                            // warnings that users should almost certainly see at least once
using Sawyer::Message::ERROR;                           // recoverable errors, possibly followed by throw
using Sawyer::Message::FATAL;                           // errors which are immediately followed by exit, abort, or similar

using Sawyer::Message::Stream;                          // one message stream, e.g., 'mlog[INFO]'
using Sawyer::Message::Facility;                        // collection of related streams, e.g., 'mlog'

/** Default destination for ROSE diagnostics.  The user may set this explicitly before rose::Diagnostics::initialize is called,
 *  otherwise that function will create a destination that points to standard error and uses the optional
 *  rose::Diagnostics::mprefix. */
ROSE_DLL_API extern Sawyer::Message::DestinationPtr destination;

/** Default line prefix for message sinks created in ROSE. For instance, if the library needs to create a default destination
 *  (rose::Diagnostics::destination) then this prefix is used, and if null at that time then a default prefix is created and
 *  assigned to this variable.  The user may assign a prefix before calling rose::Diagnostics::initialize. */
ROSE_DLL_API extern Sawyer::Message::PrefixPtr mprefix;

/** Diagnostic facility for the ROSE library as a whole. */
ROSE_DLL_API extern Sawyer::Message::Facility mlog;

/** Collection of all registered ROSE logging facilities. This collection allows the facilities to be controlled collectively
 *  such as from the -rose:log command-line switch. */
using Sawyer::Message::mfacilities;
    
/** Initialize diagnostics-related global variables. This should be called before any of the global variables in the
 *  rose::Diagnostics name space are used.  It doesn't hurt to call this more than once. */
void initialize();

/** Returns true if diagnostics-related global variables have been initialized. */
bool isInitialized();

/** Intermediate class for printing to C++ ostreams with a printf-like API.
 *
 *  Users seldom declare objects of this type directly, but rather use the rose::Diagnostics::mfprintf function or the mprintf
 *  macro.
 *
 *  @sa rose::Diagnostics::mfprintf */
class StreamPrintf {
    std::ostream &stream;
public:
    StreamPrintf(std::ostream &stream): stream(stream) {}
#ifdef _MSC_VER
    int operator()(const char *fmt, ...);
#else
    int operator()(const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#endif
};

/** Print to a C++ stream using a printf-like API.
 *
 *  The mfprintf function is a partial function whose return value is the real function. It's argument can be any C++
 *  <code>std::ostream</code> although it's intended mainly for Sawyer::Message::Stream streams.  It's used like this:
 *
 * @code
 *  uint64_t va = ...;
 *  // C++ way of emitting a message
 *  mlog[DEBUG] <<"address is " <<StringUtility::addrToString(va) <<"\n";
 *  // C way of emitting a message
 *  mfprintf(mlog[DEBUG])("address is 0x" PRIx64 "\n", va);
 *  // Shortcut macro
 *  mprintf("address is 0x" PRIx64 "\n", va);
 * @endcode
 *
 * The @c mprintf macro always uses <code>mlog[DEBUG]</code> without any name qualification in order to resolve to the most
 * locally defined @c mlog.  Therefore, a "using namespace rose::Diagnostics" probably needs to be in effect. */
StreamPrintf mfprintf(std::ostream &stream);

// See mfprintf. "mlog" must be resolved at point where macro is expanded.
#define mprintf rose::Diagnostics::mfprintf(mlog[rose::Diagnostics::DEBUG]) 

} // namespace
} // namespace

#endif

