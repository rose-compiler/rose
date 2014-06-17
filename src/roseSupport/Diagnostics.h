// Stuff related to ROSE diagnostics
#ifndef ROSE_Diagnostics_H
#define ROSE_Diagnostics_H

#include "Assert.h"                                     // Sawyer::Assert
#include "Message.h"                                    // Sawyer::Message

namespace rose {

/** Controls diagnostic messages from ROSE.
 *
 *  ROSE uses the free Sawyer library to emit diagnostic messages for events such as debugging, program tracing, informational
 *  messages, warning and error messages, progress reports, and runtime logic assertions. The Sawyer message support can be
 *  found in the Sawyer::Message name space in "Message.h", and the run-time logic assertions can be found in the
 *  Sawyer::Assert name space in "Assert.h" (which also defines a number of C preprocessor macros whose names begin with
 *  "ASSERT_".
 *
 *  Sawyer supports multiple instances of messaging facilities (Sawyer::Message::Facility) each of which defines a std::ostream
 *  object for each of a number of message importance levels.  ROSE defines one global library-wide facility,
 *  <code>Diagnostics::log</code> whose string name (used in output) is simply "rose".  Software layers within ROSE may define
 *  their own facilities and give them names indicative of the software layer.  All these facilities are then grouped together
 *  into a single Sawyer::Message::Facilities object, Diagnostics::facilities, so they can be controlled collectively or
 *  individually from the ROSE command-line (e.g., the frontend() call) via "-rose:log" switch.
 *
 * @section usage Command-line usage
 *
 *  ROSE looks for the command-line switch "-rose:log=<em>WHAT</em>".  If <em>WHAT</em> is the word "help" then usage
 *  information is displayed; if <em>WHAT</em> is the word "list" then log settings are displayed.  Otherwise, <em>WHAT</em> is
 *  expected to be a string to pass to the Sawyer::Message::Facilities::control() function.  In short, the string is a
 *  comma-separated list of importance levels to enable (or disable when preceded by "!").  Importance levels can also be
 *  enclosed in parentheses and preceded by a facility name to restrict the settings to the specified facility.  For instance,
 *  if one wanted to turn off INFO messages for all facilities, and then turn on TRACE and DEBUG messages for the BinaryLoader,
 *  he would use "-rose:log='!info, BinaryLoader(trace, debug)'".  The single quotes are to prevent the shell from doing what
 *  it normally does for exclamation marks, spaces, punctuation, etc.
 *
 *  The "-rose:log" switch may appear multiple times on the command-line, and they are processed in the order they appear.
 *  This allows for enabling and disabling various diagnostics and then querying to verify that they are in the desired state:
 *
 * @section adding Adding a facility to ROSE
 *
 *  As an example, let's say that a programmer wants to convert the BinaryLoader class to use its own logging facilities.  The
 *  first step is to declare a class data member for the facility and default construct the object at its definition in the .C
 *  file:
 *
 * @code
 *  // in BinaryLoader.h
 *  #include "Diagnostics.h"   // rose::Diagnostics
 *
 *  class BinaryLoader {
 *      ...
 *  protected:
 *      static Sawyer::Message::Facility log;
 *  };
 * @endcode
 *
 * @code
 *  // in BinaryLoader.C
 *  #include "BinaryLoader.h"
 *  Sawyer::Message::Facility BinaryLoader::log;
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
 *          log = Sawyer::Message::Facility("BinaryLoader", Diagnostics::destination);
 *          Diagnostics::facilities.insert(log);
 *      }
 *  }
 * @endcode
 *
 *  In the above code, "BinaryLoader" is the name by which this facility will be known to the "-rose:log" command-line switch.
 *  It may contain symbol names, periods, and "::" and should generally be the same as the name of the software layer which
 *  it serves.  The Diagnostics::destination is a Sawyer message plumbing lattice that controls where the message ultimately
 *  ends up (standard error is the default, but it might also be files, the syslog daemon, a database, etc).
 *
 *  The final step is to ensure that the BinaryLoader::initDiagnostics() is called from Diagnostics::initialize(). This
 *  requires editing  Diagnostics.C to include BinaryLoader.h and the call to BinaryLoader::initDiagnostics().
 *
 * @section usage Using a facility in ROSE
 *
 *  Generally speaking, all one needs to do to use a facility is to write a C++ std::ostream-style output statement whose
 *  left-hand operand is a Sawyer::Message::Stream.  Since streams are held in a Sawyer::Message::Facility and since facilities
 *  usually have the C++ name "log", the statement will look something like this:
 *
 * @code
 *  using Diagnostics;
 *  log[INFO] <<"loading \"" <<filename <<"\"\n";
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
 *  If the C math library is declared and "using Sawyer::Message" is in effect, the symbol "log" will be ambiguous and will
 *  need to be qualified.
 *
 *  Sometimes one wants the right hand side of the output statements to be evaluated only in certain circumstances for
 *  performance reasons, and there are multiple ways to do that.  If you already use conditional compilation or "if" statements
 *  to generate diagnostic output, continue to do the same.  You can also use an "if" statement whose condition is the
 *  diagnostic stream since streams evaluated in a Boolean context return true or false depending on whether the stream is
 *  enabled or disabled:
 *
 * @code
 *  if (log[INFO])
 *      log[INFO] <<"loading \"" <<filename <<"\"\n";
 * @endcode
 *
 *  Sawyer also has a SAWYER_MESG() macro that's used like this:
 *
 * @code
 *  SAWER_MESG(log[INFO]) <<"loading \"" <<filename <<"\"\n";
 * @endcode
 *
 * @section partial Partial messages
 *
 *  A partial message is any message for which the linefeed, "\n", hasn't been inserted.  If a message stream is unbuffered and
 *  another message is emitted to a different stream and ends up going to the same ultimate destination, then any previous
 *  partial message is interrupted, usually by appending "...", and re-emitted later when more text is added.
 *
 *  Since each message stream creates messages sequentially in an incremental manner, we sometimes want to "save" a partial
 *  message, allow other messages to be emitted from the same stream, and then complete the original message later.  This most
 *  often happens when one function creates a partial message, and then calls another function that might use the same stream
 *  before the partial message is completed.  The best way to accomplish this is by creating a new, temporary message stream
 *  and moving any partial messages to the new stream.  This is done with what looks like a copy constructor:
 *
 * @code
 *  Stream m1(log[INFO] <<"loading \"" <<filename <<"\""); //note no "\n"
 *  do_other_stuff_that_might_emit_info_messages();
 *  m1 <<"; done loading.\n"; //original message completed now
 * @endcode
 *
 *  The temporary stream inherits the message, properties, enabled state, and destination from the source stream, freeing the
 *  source stream to be used for other purposes.  Another usage for stream copying is to have a temporary name that's shorter,
 *  especially if name qualification is used, and to change the name of the facility to be the same as the function:
 *
 * @code
 *  void BinaryLoader::foo() {
 *      Sawyer::Message::Stream trace(Diagnostics::log[Sawyer::Message::TRACE]);
 *      trace.facilityName("BinaryLoader::foo");
 *      trace <<"now I have my own stream";
 *      do_other_stuff()
 *      trace <<" and nobody can interfere\n";
 *  }
 * @endcode
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


/** Default destination for ROSE diagnostics. */
extern Sawyer::Message::DestinationPtr destination;

/** Diagnostic facility for the ROSE library as a whole. */
extern Sawyer::Message::Facility log;

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

