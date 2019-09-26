// Stuff related to ROSE diagnostics
#ifndef ROSE_Diagnostics_H
#define ROSE_Diagnostics_H

#include "Sawyer/Assert.h"
#include "Sawyer/Message.h"

// How failed assertions behave
#define ROSE_ASSERTION_ABORT    1                       // call abort()
#define ROSE_ASSERTION_EXIT     2                       // call exit(1)
#define ROSE_ASSERTION_THROW    3                       // throw Rose::Diagnostics::FailedAssertion

namespace Rose {

/** Controls diagnostic messages from ROSE.
 *
 *  ROSE uses the free [Sawyer](http://github.com/matzke1/sawyer) library to emit diagnostic messages for events such as
 *  debugging, program tracing, informational messages, warning and error messages, progress reports, and runtime logic
 *  assertions. The Sawyer message support can be found in the Sawyer::Message name space in "Sawyer/Message.h", and the
 *  run-time logic assertions can be found in the Sawyer::Assert name space in "assert/Assert.h" (which also defines a number
 *  of C preprocessor macros whose names begin with "ASSERT_".
 *
 *  Sawyer supports multiple instances of messaging facilities (@ref Sawyer::Message::Facility or @ref
 *  Rose::Diagnostics::Facility) each of which defines a <code>std::ostream</code> object for each of a number of message
 *  importance levels.  ROSE defines one global library-wide facility, @ref Rose::Diagnostics::mlog whose string name (used in
 *  output) is simply "rose".  Software layers within ROSE may define their own facilities and give them names indicative of
 *  the software layer, like "Rose::BinaryAnalysis::StackDelta. Tools that use ROSE can also define and register their own
 *  facilities.  All these facilities are then grouped together into a single @ref Sawyer::Message::Facilities object, @ref
 *  Rose::Diagnostics::mfacilities, so they can be controlled collectively or individually from the ROSE command-line (e.g.,
 *  the @c frontend call).
 *
 * @section usage Command-line usage
 *
 *  ROSE looks for the command-line switch (e.g., "--log") that takes one argument: <em>what</em>".  If <em>what</em> is the
 *  word "help" then usage information is displayed; if <em>what</em> is the word "list" then log settings are displayed.
 *  Otherwise, <em>what</em> is expected to be a string to pass to the @ref Rose::Diagnostics::mfacilities.control function.
 *  See the output from any tool that supports "--log help" for details about the syntax.
 *
 * @section library_diagnostics Adding a facility to ROSE
 *
 *  Note: this section is about adding a new logging facility to the ROSE library proper. Adding a facility to a tool that uses
 *  ROSE is simpler and is described in a later section.
 *
 *  As an example, let's say that a programmer wants to convert the BinaryLoader class to use its own logging facilities.  The
 *  first step is to declare a static data member for the facility. Adding a logging facility to a namespace or file is similar
 *  except the @c mlog would be declared in the namespace or as a file-scope (static or anonymous namespace) variable.
 *
 * @code
 *  // in BinaryLoader.h
 *  #include "Diagnostics.h"   // Rose::Diagnostics
 *
 *  class BinaryLoader {
 *      ...
 *  protected:
 *      static Rose::Diagnostics::Facility mlog; // a.k.a., Sawyer::Message::Facility
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
 *  Rose::Diagnostics::Facility BinaryLoader::mlog;
 * @endcode
 *
 *  The third step is to initialize the @c mlog static variable after we're sure that the C++ runtime has been initialized. We
 *  can do this by defining a class method (a.k.a., static function member) that can be called from ROSE's initialization
 *  phase.  This is the point at which the mlog is given a name and connected to a destination(s).  The name is usually the
 *  fully qualified name of the component for which the @c mlog object serves as the logging facility, and may contain typical
 *  C++ symbol names, dots, and "::".  This name is used to enable/disable the facility from the command-line, and will show up
 *  as part of the output for any message emitted using this facility.  It is recommended that you also provide a comment in
 *  order to help people that are not ROSE developers to use ROSE-based tools. The comment should start with a gerund (verb
 *  ending with "ing"), be uncapitalized, and unpunctuated.
 *
 *  The second, optional half of this step is to register the facility with the ROSE library so it can be controlled from the
 *  command-line. Although unregistered facilities are still useful, components of the ROSE library should almost always
 *  register their facility. The easiest way to do this consistently is to use @ref initAndRegister like this:
 *
 * @code
 *  // class method (static member function) in BinaryLoader.C
 *  void BinaryLoader::initDiagnostics() {
 *      static bool initialized = false;
 *      if (!initialized) {
 *          initialized = true;
 *          Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::BinaryLoader");
 *          mlog.comment("mapping files into virtual memory");
 *      }
 *  }
 * @endcode
 *
 *  If the class, namespace, or file containing you @c initDiagnostics function is conditionally compiled (e.g., binary
 *  analysis is not always enabled in ROSE), you'll want to add a dummy version of @c initDiagnostics that does nothing
 *  (preferrable to using conditional compilation in the next step).
 *
 *  The fourth and final step is to add a call to <code>BinaryLoader::initDiagnostics</code> from @ref
 *  Diagnostics::initialize. This function is defined in "src/roseSupport/Diagnostics.C". You probably don't need to include
 *  your entire header file in Diagnostics.C; a declarations should be sufficient and faster to compile.
 *
 * @section tool_diagnostics Adding a facility to a tool
 *
 *  Tools that are built on top of the ROSE library can use the same Sawyer messaging support, and the tool's Facility objects
 *  can be registered with ROSE and thus controlled along with the library's facilities.  Doing so is even easier than using
 *  the facility in part of the ROSE library: declare the facility, and initialize and register it.  Tools can create as many
 *  facilities as they like, although the description below assumes only one.
 *
 *  The facility is usually declared as a static object so it can be available to all parts of the tool.  As with static
 *  facilities in the ROSE library itself (described above), it should be only default constructed.  Also, instead of using
 *  namespace @ref Rose::Diagnostics, the tool can optionally use namespace @ref Sawyer::Message::Common.  This avoids the
 *  ambiguity for @c mlog that would be created by importing @ref Rose::Diagnostics or @ref Sawyer::Message (the types and
 *  enums in these two namespaces are equivalent).
 *
 * @code
 *  #include <rose/Diagnostics.h>
 *  using namespace Sawyer::Message::Common; // if you want unqualified DEBUG, WARN, ERROR, FATAL, etc.
 *  Sawyer::Message::Facility mlog; // a.k.a., Rose::Diagnostics::Facility
 * @endcode
 *
 *  The second step, initialization and registration, is usually near the beginning of @c main right after initializing the
 *  ROSE library but before command-line processing.
 *
 * @code
 *  int main(int argc, char *argv[]) {
 *      ROSE_INITIALIZE;
 *      mlog = Rose::Diagnostics::initAndRegister(&mlog, "name-of-my-tool");
 * @endcode
 *
 *  If you want to globally adjust which levels of diagnostics are output by default (later modified by the command-line) you
 *  can do that by passing a string to the same function that's used by the command-line processing, such as:
 *
 * @code
 *  Sawyer::Message::mfacilities.control("none,>=info");
 * @endcode
 *
 *  Although initialization is required, the registering step is optional. Registering causes the tool's diagnostics to be
 *  conrollable from ROSE's command-line switches.  If you want to only initialize and not register, then use this:
 *
 * @code
 *  int main(int argc, char *argv[]) {
 *      ROSE_INITIALIZE;
 *      mlog = Sawyer::Message::Facility("name-of-my-tool", Rose::Diagnostics::destination);
 * @endcode
 *
 *
 * @section usage Using a facility in the ROSE library or tools
 *
 *  When using a message facility within the ROSE library source code, it is generally desirable to have a "using namespace
 *  @ref Rose::Diagnostics" in effect.  Not only does this alleviate the need to qualify the important levels (e.g.,
 *  <code>INFO</code>, <code>WARN</code>, etc), but it also brings @ref Rose::Diagnostics::mlog, the library-wide logging
 *  facility, into scope.  Doing so will allow any software component to perform logging using the library-wide facility, and
 *  once a more local @c mlog is declared the more local symbol is automatically used.
 *
 *  When using a message facility within a tool, importing the @ref Sawyer::Message::Common instead of @ref Rose::Diagnostics
 *  will prevent an ambiguity between the tools global @c mlog and @ref Rose::Diagnostics::mlog. You can do which ever you
 *  prefer.
 *
 *  Regardless of whether you're writing a ROSE library component or a tool, all one needs to do to use a facility is to write
 *  a C++ std::ostream-style output statement whose left-hand operand is a @ref Sawyer::Message::Stream.  Since streams are
 *  held in a @ref Sawyer::Message::Facility and since facilities usually have the C++ name "mlog", the statement will look
 *  something like this:
 *
 * @code
 *  using Rose::Diagnostics; // or using Sawyer::Message::Common
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
 *  using namespace Rose::Diagnostics;
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
 *  Not only does this reduce typing a little, but since the function is using its own private message stream, partial
 *  messages emitted to that stream won't interfere with partial messages emitted to <code>mlog[DEBUG]</code> by called
 *  functions (see next section).
 *
 *  Another debugging trick is to create a local stream like above, and then explicitly enable or disable it based on some
 *  condition you're trying to debug:
 *
 * @code
 *  using namespace Rose::Diagnostics;
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
 *  Sawyer::Stopwatch timer;
 *  do_other_stuff_that_might_emit_info_messages();
 *  m1 <<"; took " <<timer <<" seconds.\n"; //original message completed now
 *  // you may continue to use m1 for additional messages...
 * @endcode
 *
 *  The documentation for @ref Sawyer::Message has additional hints and examples.
 *
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

/** Default destination for ROSE diagnostics.  The user may set this explicitly before Rose::Diagnostics::initialize is called,
 *  otherwise that function will create a destination that points to standard error and uses the optional
 *  Rose::Diagnostics::mprefix. */
ROSE_DLL_API extern Sawyer::Message::DestinationPtr destination;

/** Default line prefix for message sinks created in ROSE. For instance, if the library needs to create a default destination
 *  (Rose::Diagnostics::destination) then this prefix is used, and if null at that time then a default prefix is created and
 *  assigned to this variable.  The user may assign a prefix before calling Rose::Diagnostics::initialize. */
ROSE_DLL_API extern Sawyer::Message::PrefixPtr mprefix;

/** Diagnostic facility for the ROSE library as a whole. */
ROSE_DLL_API extern Sawyer::Message::Facility mlog;

/** Collection of all registered ROSE logging facilities. This collection allows the facilities to be controlled collectively
 *  such as from the -rose:log command-line switch. */
using Sawyer::Message::mfacilities;
    
/** Initialize diagnostics-related global variables. This should be called before any of the global variables in the
 *  Rose::Diagnostics name space are used.  It doesn't hurt to call this more than once. */
ROSE_DLL_API void initialize();

/** Returns true if diagnostics-related global variables have been initialized. */
ROSE_DLL_API bool isInitialized();

/** Initialize and register a logging facility.
 *
 *  Initializes the specified facility by giving it a name and pointing it to the same place as the other ROSE diagnostic
 *  streams. Then register the facility with ROSE's global list of facilities so it can be controlled from the command-line.
 *  Registering a facility does not copy it--it only saves a reference to it. Therefore, either facilties should be statically
 *  declared or they should be de-registered before they go out of scope. */
ROSE_DLL_API void initAndRegister(Facility *mlog, const std::string &name);

// [Robb P Matzke 2017-02-16]: deprecated: The version that takes a pointer first argument is a better design because it gives
// the user a better clue that we're saving a reference rather than a copy. Note: ROSE_DEPRECATED not defined here.
ROSE_DLL_API void initAndRegister(Facility &mlog, const std::string &name) SAWYER_DEPRECATED("use a pointer for the first argument");

/** Deregister a facility.
 *
 *  Do this before your facility goes out of scope.  In general, you don't need to deregister a Facility object from a
 *  Facilities collection when the Facility object is destroyed because Sawyer will notice the destruction
 *  automatically. However, the recognition is heuristic based on magic numbers manipulated by constructors and destructors and
 *  could potentially fail.  The general safe, rule is that if a registered Facility object is destroyed other than at program
 *  exit, you should explicitly deregister it before it's destroyed. */
ROSE_DLL_API void deregister(Facility *mlog);

/** Intermediate class for printing to C++ ostreams with a printf-like API.
 *
 *  Users seldom declare objects of this type directly, but rather use the Rose::Diagnostics::mfprintf function or the mprintf
 *  macro.
 *
 *  @sa Rose::Diagnostics::mfprintf */
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
 * locally defined @c mlog.  Therefore, a "using namespace Rose::Diagnostics" probably needs to be in effect. */
StreamPrintf mfprintf(std::ostream &stream);

// See mfprintf. "mlog" must be resolved at point where macro is expanded.
#define mprintf Rose::Diagnostics::mfprintf(mlog[Rose::Diagnostics::DEBUG]) 

} // namespace
} // namespace

#endif

