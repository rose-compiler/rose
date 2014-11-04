#ifndef Sawyer_H
#define Sawyer_H

#include <boost/cstdint.hpp>
#include <cstdio>
#include <string>

/** @mainpage
 *
 *  %Sawyer is a library that provides the following:
 *
 *  @li Conditionally enable streams for program diagnostics.  These are C++ <code>std::ostreams</code> organized by software
 *      component and message importance and which can be enabled/disabled with a simple language. A complete plumbing system
 *      similar to Unix file I/O redirection but more flexible can do things like multiplexing messages to multiple locations
 *      (e.g., stderr and syslogd), rate-limiting, colorizing on ANSI terminals, and so on. See Sawyer::Message for details.
 *
 *  @li Logic assertions in the same vein as <tt>\<cassert></tt> but using the diagnostic streams and multi-line output to make
 *      them more readable.  See Sawyer::Assert for details.
 *
 *  @li Progress bars for text-based output using the diagnostic streams so that progress bars interact sanely with other
 *      diagnostic output.  See Sawyer::ProgressBar for details.
 *
 *  @li Command-line parsing to convert program switches and their arguments into C++ objects with the goal of being able to
 *      support nearly any command-line style in use, including esoteric command switches from programs like tar and
 *      dd. Additionally, documentation can be provided in the C++ source code for each switch and Unix man pages can be
 *      produced. See Sawyer::CommandLine for details.
 *
 *  @li Container classes: @ref Sawyer::Container::Graph "Graph", storing vertex and edge connectivity information along with
 *      user-defined values attached to each vertex and edge, sequential ID numbers, and constant time complexity for most
 *      operations; @ref Sawyer::Container::IndexedList "IndexedList", a combination list and vector having constant time
 *      insertion and erasure and constant time lookup-by-ID; @ref Sawyer::Container::Interval "Interval" represents integral
 *      intervals including empty and whole intervals; @ref Sawyer::Container::IntervalSet "IntervalSet" and @ref
 *      Sawyer::Container::IntervalMap "IntervalMap" similar to STL's <code>std::set</code> and <code>std::map</code>
 *      containers but optimized for cases when very large numbers of keys are adjacent; @ref Sawyer::Container::Map "Map",
 *      similar to STL's <code>std::map</code> but with an API that's consistent with other containers in this library; @ref
 *      Sawyer::Container::BitVector "BitVector" bit vectors with operations defined across index intervals.  These can be
 *      found in the Sawyer::Container namespace.
 *
 *   @li Miscellaneous: @ref Sawyer::PoolAllocator "PoolAllocator" to allocate memory from large pools rather than one
 *       object at a time; and @ref Sawyer::SmallObject "SmallObject", a base class for objects that are only a few bytes;
 *       @ref Sawyer::Stopwatch "Stopwatch" for high-resolution elapsed time; @ref Sawyer::Optional "Optional" for optional
 *       values.
 *
 *  Design goals for this library can be found in the [Design goals](group__design__goals.html) page.
 *
 *  Other things on the way but not yet ready:
 *
 *  @li A simple, extensible, terse markup language that lets users write documentation that can be turned into TROFF, HTML,
 *      Doxygen, PerlDoc, TeX, etc. The markup language supports calling of C++ functors to transform the text as it is
 *      processed.
 *
 *  <b>Good starting places for reading documentation are the [namespaces](namespaces.html).</b> */


/** @defgroup design_goals Library design goals
 *
 *  Goals that influence the design of this library.
 *
 *  @li The API should be well documented and tested.  Every public symbol is documented with doxygen and includes all
 *      pertinent information about what it does, how it relates to other parts of the library, restrictions and caveats, etc.
 *      This kind of information is not evident from the C++ interface itself and is often omitted in other libraries'
 *      documentation.
 *
 *  @li The library should be easy to use yet highly configurable. Common things should be simple and terse, but less common
 *      things should still be possible without significant work. Users should not have to worry about who owns what
 *      objects--the library uses reference counting pointers to control deallocation.  The library should use a consistent
 *      naming scheme. It should avoid extensive use of templates since they're a common cause of difficulty for beginners.
 *
 *  @li The library should be familiar to experienced C++ programmers. It should use the facilities of the C++ language, C++
 *      idioms, and an object oriented design.  The API should not look like a C library being used in a C++ program, and the
 *      library should not be concerned about being used in languages that are not object oriented.
 *
 *  @li The library should be safe to use.  Errors should be handled in clear, recoverable, and predictable ways. The library
 *      should make every attempt to avoid situations where the user can cause a non-recoverable fault due to misunderstanding
 *      the API.
 *
 *  @li Functionality is more important than efficiency. Every attempt is made to have an efficient implementation, but when
 *      there is a choice between functionality and efficiencey, functionality wins. */


/** @defgroup smart_pointers Reference counting smart pointers
 *
 *  Pointers that automatically delete the underlying object.
 *
 *  The library makes extensive use of referencing counting smart pointers.  It uses the following paradigm consistently for
 *  any class that intends to be reference counted.
 *
 *  @li The class shall mark all of its normal C++ constructors as having protected access.  This is to prevent users from
 *      allocating such objects statically or on the stack, yet allowing subclasses to use them.
 *
 *  @li For each class their shall be defined a "Ptr" type which is the smart pointer type for the class.  Class templates
 *      define this type as a public member; non-template classes may define it as a public member and/or in the same namespace
 *      as the class. In the latter case, the type name will be "Ptr" appended to the class name.
 *
 *  @li The class shall have a static <code>instance</code> method corresponding to each C++ constructor. Each such
 *      class method takes the same arguments as one of the constructors, allocates and constructs and object with
 *      <code>new</code>, and returns a <code>Ptr</code>. These methods are usually public.
 *
 *  @li The class shall have a public destructor only for the sake of the smart pointer.
 *
 *  @li If a class hierarchy needs virtual constructors they shall be named <code>create</code>.
 *
 *  @li If a class needs virtual copy constructors they shall be named <code>copy</code>.
 *
 *  @li The class shall have a factory function corresponding to each public <code>instance</code> method.
 *
 *  @li Factory functions shall have the same name as the class, but an initial lower-case letter.
 *
 *  A simple example:
 *
 * @code
 *  class SomeClass {
 *      int someData_;           // underscores are used for private data members
 *
 *  protected:
 *      SomeClass(): someData_(0) {}
 *
 *      explicit SomeClass(int n): someData_(n) {}
 *
 *  public:
 *      typedef Sawyer::SharedPointer<SomeClass> Ptr;
 *
 *      static Ptr instance() {
 *          return Ptr(new SomeClass);
 *      }
 *
 *      static Ptr instance(int n) {
 *          return Ptr(new SomeClass(n));
 *      }
 *  };
 *
 *  SomeClass::Ptr someClass() {
 *      return SomeClass::instance();
 *  }
 *
 *  SomeClass::Ptr someClass(int n) {
 *      return SomeClass::instance(n);
 *  }
 * @endcode */


/** @defgroup class_properties Class properties
 *
 *  Data members that that store a simple value.
 *
 *  Class data members that act like user-accessible properties are declared with private access. As with all private data
 *  members, they end with an underscore.  The class provides a pair of methods for accessing each property--one for reading the
 *  property and one for modifying the property. Some properties are read-only in which case only the writer is provided.
 *
 *  All writer properties return a reference to the object that is modified so that property settings can be chained.  If the
 *  class uses the reference-counting smart-pointer paradigm, then a pointer to the object is returned instead. (See @ref
 *  smart_pointers).
 *
 * @code
 *  class SomeClass {
 *      int someProperty_;
 *  public:
 *      int someProperty() const {
 *          return someProperty_;
 *      }
 *      SomeClass& someProperty(int someProperty) {
 *          someProperty_ = someProperty;
 *          return *this;
 *      }
 *  };
 * @endcode */

#ifdef BOOST_WINDOWS
// FIXME[Robb Matzke 2014-06-18]: get rid of ROSE_UTIL_EXPORTS; cmake can only have one DEFINE_SYMBOL
#   if defined(SAWYER_DO_EXPORTS) || defined(ROSE_UTIL_EXPORTS) // defined in CMake when compiling libsawyer
#       define SAWYER_EXPORT __declspec(dllexport)
#       if _MSC_VER
#           define SAWYER_EXPORT_NORETURN __declspec(dllexport noreturn)
#       else
            // MinGW complains about __declspec(dllexport noreturn), so use only __declspec(dllexport) instead.
#           define SAWYER_EXPORT_NORETURN __declspec(dllexport)
#       endif
#   else
#       define SAWYER_EXPORT __declspec(dllimport)
#       define SAWYER_EXPORT_NORETURN __declspec(noreturn)
#   endif
#else
#   define SAWYER_EXPORT /*void*/
#   define SAWYER_EXPORT_NORETURN /*void*/
#endif


/** Name space for the entire library.  All %Sawyer functionality except for some C preprocessor macros exists inside this
 * namespace.  Most of the macros begin with the string "SAWYER_". */
namespace Sawyer {

/** Explicitly initialize the library. This initializes any global objects provided by the library to users.  This happens
 *  automatically for many API calls, but sometimes needs to be called explicitly. Calling this after the library has already
 *  been initialized does nothing. The function always returns true. */
SAWYER_EXPORT bool initializeLibrary();

/** True if the library has been initialized. */
SAWYER_EXPORT extern bool isInitialized;

/** Portable replacement for ::strtoll
 *
 *  Microsoft doesn't define this function, so we define it in the Sawyer namespace. */
SAWYER_EXPORT boost::int64_t strtoll(const char*, char**, int);

/** Portable replacement for ::strtoull
 *
 *  Microsoft doesn't define this function, so we define it in the Sawyer namespace. */
SAWYER_EXPORT boost::uint64_t strtoull(const char*, char**, int);

/** Reads one line of input from a file.
 *
 *  Returns one line, including any line termination.  Returns an empty string at the end of the file. */
SAWYER_EXPORT std::string readOneLine(FILE*);

/** Semi-portable replacement for popen. */
SAWYER_EXPORT FILE *popen(const std::string&, const char *how);

/** Semi-portable replacement for pclose. */
SAWYER_EXPORT int pclose(FILE*);

/** Generate a sequential name.
 *
 *  A new string is generated each time this is called. */
SAWYER_EXPORT std::string generateSequentialName(size_t length=3);

} // namespace

// Define only when we have the Boost Chrono library, which was first available in boost-1.47.
//#define SAWYER_HAVE_BOOST_CHRONO


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Compiler portability issues
//
// The following macros are used to distinguish between different compilers:
//     _MSC_VER         Defined only when compiled by Microsoft's MVC C++ compiler. This macro is predefined by Microsoft's
//                      preprocessor.
//
// The following macros are used to distinguish between different target environments, regardless of what compiler is being
// used or the environment which is doing the compiling.  For instance, BOOST_WINDOWS will be defined when using the MinGW
// compiler on Linux to target a Windows environment.
//     BOOST_WINDOWS    The Windows API is present.  This is defined (or not) by including <boost/config.hpp>.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# define SAWYER_ATTR_UNUSED /*unused*/
# define SAWYER_ATTR_NORETURN /*noreturn*/
# define SAWYER_PRETTY_FUNCTION __FUNCSIG__
# define SAWYER_MAY_ALIAS /*void*/
# define SAWYER_STATIC_INIT /*void*/

// MVC doesn't support stack arrays whose size is not known at compile time.  We fudge by using an STL vector, which will be
// cleaned up propertly at end of scope or exceptions.
# define SAWYER_VARIABLE_LENGTH_ARRAY(TYPE, NAME, SIZE) \
    std::vector<TYPE> NAME##Vec_(SIZE);                 \
    TYPE *NAME = &(NAME##Vec_[0]);

#else
# define SAWYER_ATTR_UNUSED __attribute__((unused))
# define SAWYER_ATTR_NORETURN __attribute__((noreturn))
# define SAWYER_PRETTY_FUNCTION __PRETTY_FUNCTION__
# define SAWYER_MAY_ALIAS __attribute__((may_alias))

// Sawyer globals need to be initialized after the C++ standard runtime
# define SAWYER_STATIC_INIT __attribute__((init_priority(65534)))

# define SAWYER_VARIABLE_LENGTH_ARRAY(TYPE, NAME, SIZE) \
    TYPE NAME[SIZE];

#endif

#define SAWYER_CONFIGURED /*void*/

#endif
