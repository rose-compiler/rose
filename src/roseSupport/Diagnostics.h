// Stuff related to ROSE diagnostics
#ifndef ROSE_Diagnostics_H
#define ROSE_Diagnostics_H

#include "sawyer/Assert.h"
#include "sawyer/Message.h"

namespace rose {

/** Controls diagnostic messages from ROSE.
 *
 *  ROSE uses the free [Sawyer](http://github.com/matzke1/sawyer) library to emit diagnostic messages for events such as
 *  debugging, program tracing, informational messages, warning and error messages, progress reports, and runtime logic
 *  assertions. The Sawyer message support can be found in the Sawyer::Message name space in "sawyer/Message.h", and the
 *  run-time logic assertions can be found in the Sawyer::Assert name space in "assert/Assert.h" (which also defines a number
 *  of C preprocessor macros whose names begin with "ASSERT_".
 *
 *  Sawyer supports multiple instances of messaging facilities (Sawyer::Message::Facility) each of which defines a std::ostream
 *  object for each of a number of message importance levels.  ROSE defines one global library-wide facility,
 *  <code>Diagnostics::mlog</code> whose string name (used in output) is simply "rose".  Software layers within ROSE may define
 *  their own facilities and give them names indicative of the software layer.  All these facilities are then grouped together
 *  into a single Sawyer::Message::Facilities object, Diagnostics::facilities, so they can be controlled collectively or
 *  individually from the ROSE command-line (e.g., the frontend() call) via "-rose:log" switch.
 *
 * @section usage Command-line usage
 *
 *  ROSE looks for the command-line switch "-rose:log <em>WHAT</em>".  If <em>WHAT</em> is the word "help" then usage
 *  information is displayed; if <em>WHAT</em> is the word "list" then log settings are displayed.  Otherwise, <em>WHAT</em> is
 *  expected to be a string to pass to the Diagnostics::facilities.control() function.  In short, the string is a
 *  comma-separated list of importance levels to enable (or disable when preceded by "!").  Importance levels can also be
 *  enclosed in parentheses and preceded by a facility name to restrict the settings to the specified facility.  For instance,
 *  if one wanted to turn off INFO messages for all facilities, and then turn on TRACE and DEBUG messages for the BinaryLoader,
 *  he would use "-rose:log '!info, BinaryLoader(trace, debug)'".  The single quotes are to prevent the shell from doing what
 *  it normally does for exclamation marks, spaces, and other punctuation.  See the doxygen documentation for
 *  Sawyer::Message::Facilities::control() for details.
 *
 *  The "-rose:log" switch may appear multiple times on the command-line, and they are processed in the order they appear.
 *  This allows for enabling and disabling various diagnostics and then querying to verify that they are in the desired state:
 *
 * @section adding Adding a facility to ROSE
 *
 *  As an example, let's say that a programmer wants to convert the BinaryLoader class to use its own logging facilities.  The
 *  first step is to declare a class (static) data member for the facility and give the facility a name.  The facility is
 *  immediately usable, although it is not yet fully initialized or controllable from the command-line.
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
 *  If we consistently name message facilities "mlog" then any unqualified reference to "mlog" in the source code will probably
 *  be the most appropriate (most narrowly scoped) facility.  Adding a new facility to a software component that already uses
 *  the rose-wide "mlog" is mostly a matter of defining a more narrowly scoped "mlog", and the existing code will start using
 *  it (you probably also need to search for places where "mlog" usage was qualified). Specifically, we avoid using "log" as
 *  the name of any logging facility because it can conflict on some systems with the logorithm function "log".
 *
 *  Then define the "mlog" and give it a string name.  The string name, "BinaryLoader", is the name by which this facility will
 *  be known to the "-rose:log" command-line switch.  It may contain symbol names, periods, and "::" and should generally be
 *  the same as the name of the software layer which it serves.  We normally omit "rose::" from the string name.
 *
 * @code
 *  // in BinaryLoader.C
 *  #include "BinaryLoader.h"
 *  Sawyer::Message::Facility BinaryLoader::mlog("BinaryLoader");
 * @endcode
 *
 *  The second step is to cause the facility to be initialized so that messages sent to the facility go to the same place
 *  as the rest of ROSE diagnostics (e.g., standard error).  We do this by defining an initDiagnostics() class method:
 *
 * @code
 *  // class method in BinaryLoader.C
 *  void BinaryLoader::initDiagnostics() {
 *      static bool initialized = false;
 *      if (!initialized) {
 *          initialized = true;
 *          mlog.initStreams(Diagnostics::destination);
 *          Diagnostics::facilities.insert(mlog);
 *      }
 *  }
 * @endcode
 *
 *  The Diagnostics::destination is a Sawyer message plumbing lattice that controls where the message ultimately
 *  ends up (standard error is the default, but it might also be files, the syslog daemon, a database, etc).
 *
 *  The final step is to ensure that the BinaryLoader::initDiagnostics() is called from Diagnostics::initialize(). This
 *  requires editing  Diagnostics.C to include BinaryLoader.h and call BinaryLoader::initDiagnostics().
 *
 * @section usage Using a facility in ROSE
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
 *  can be registered with ROSE and thus controlled along with the library's facilities.  Doing so is a two-step process:
 *  declare the facility in the tool and then initialize and register the facility with ROSE.
 *
 *  The declaration and definition is usually at global scope for the tool in order to be available everywhere in the tool:
 *
 * @code
 *  #include <rose/Diagnostics.h>
 *  using namespace Saywer::Message::Common;
 *  Sawyer::Message::Facility mlog("toolname");
 * @endcode
 *
 *  The initialization and registration is usually near the beginning of main():
 *
 * @code
 *  rose::Diagnostics::initialize();
 *  mlog.initStreams(rose::Diagnostics::destination);
 *  rose::Diagnostics::facilities.insertAndAdjust(mlog);
 * @endcode
 *
 *  The first line ensures that the global variables in rose::Diagnostics are properly initialized. You don't need to do this
 *  if you've already initialized the ROSE library in some other manner.
 *
 *  The initStreams() method connects the tool's logging facility with ROSE's message plumbing lattice to cause the tool's
 *  messages to go to the same place(s) as ROSE library messages.
 *
 *  The insertAndAdjust() method registers the tool's logging facility with the ROSE library and enables and disables the
 *  tool's message streams so they're in the same state as the ROSE library streams (probably debug and tracing messages are
 *  disabled and info, warn, error, and fatal are enabled).
 *
 *  The tool can parse arguments in it's normal way.  It can forward the "-rose:log" arguments to the ROSE library so that the
 *  library can control the tools' message streams, or it can process its own arguments can call
 *  rose::Diagnostics::facilities.control() itself.
 */
namespace Diagnostics {

// Make Sawyer message importance levels available here. That way, we don't have to "use namespace Sawyer::Message", which
// also sucks in a lot of other stuff.
using Sawyer::Message::DEBUG;
using Sawyer::Message::TRACE;
using Sawyer::Message::WHERE;
using Sawyer::Message::INFO;
using Sawyer::Message::WARN;
using Sawyer::Message::ERROR;
using Sawyer::Message::FATAL;

using Sawyer::Message::Stream;
using Sawyer::Message::Facility;

/** Default destination for ROSE diagnostics. */
extern Sawyer::Message::DestinationPtr destination;

/** Diagnostic facility for the ROSE library as a whole. */
extern Sawyer::Message::Facility mlog;

/** Collection of all registered ROSE logging facilities. This collection allows the facilities to be controlled collectively
 *  such as from the -rose:log command-line switch. */
extern Sawyer::Message::Facilities facilities;
    
/** Initialize diagnostics-related global variables. This should be called before any of the global variables in the
 *  rose::Diagnostics name space are used.  It doesn't hurt to call this more than once. */
void initialize();

/** Returns true if diagnostics-related global variables have been initialized. */
bool isInitialized();

} // namespace
} // namespace

#endif

