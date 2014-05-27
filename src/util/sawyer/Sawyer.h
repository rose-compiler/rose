#ifndef Sawyer_H
#define Sawyer_H

/** @mainpage
 *
 *  %Sawyer is a library that provides the following:
 *
 *  @li Conditionally enable streams for program diagnostics.  These are C++ std::ostreams organized by software component
 *      and message importance and which can be enabled/disabled with a simple language. A complete plumbing system similar to
 *      Unix file I/O redirection but more flexible can do things like multiplexing messages to multiple locations (e.g.,
 *      stderr and syslogd), rate-limiting, colorizing on ANSI terminals, and so on. See Sawyer::Message for details.
 *
 *  @li Logic assertions in the same vein as \<cassert> but using the diagnostic streams and multi-line output to make them
 *      more readable.  See Sawyer::Assert for details.
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
 *      user-defined values attached to each vertex and node, sequential ID numbers, and constant time complexity for most
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
 *       @ref Sawyer::Stopwatch "Stopwatch" for high-resolution elapsed time.
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
 *  @li The library should be easy to use yet highly configurable. Common things should be simple and terse, but less common
 *      things should still be possible without significant work. Users should not have to worry about who owns what
 *      objects--the library uses reference counting pointers to control deallocation.  The library should use a consistent
 *      naming scheme. It should avoid extensive use of templates since they're a common cause of difficulty for beginners.
 *  @li The library should be familiar to experienced C++ programmers. It should use the facilities of the C++ language, C++
 *      idioms, and an object oriented design.  The API should not look like a C library being used in a C++ program, and the
 *      library should not be concerned about being used in languages that are not object oriented.
 *  @li The library should be safe to use.  Errors should be handled in clear, recoverable, and predictable ways. The library
 *      should make every attempt to avoid situations where the user can cause a non-recoverable fault due to misunderstanding
 *      the API.
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
 *  @li For each class their shall be defined a "Ptr" type which is the smart pointer type for the class.  Class templates
 *      define this type as a public member; non-template classes may define it as a public member and/or in the same namespace
 *      as the class. In the latter case, the type name will be "Ptr" appended to the class name.
 *  @li The class shall have a static <code>instance</code> method corresponding to each C++ constructor. Each such
 *      class method takes the same arguments as one of the constructors, allocates and constructs and object with
 *      <code>new</code>, and returns a <code>Ptr</code>. These methods are usually public.
 *  @li The class shall have a public destructor only for the sake of the smart pointer.
 *  @li If a class hierarchy needs virtual constructors they shall be named <code>create</code>.
 *  @li If a class needs virtual copy constructors they shall be named <code>copy</code>.
 *  @li The class shall have a factory function corresponding to each public <code>instance</code> method.
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
 *      typedef boost::shared_ptr<SomeClass> Ptr;
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


/** Name space for the entire library.  All %Sawyer functionality except for some C preprocessor macros exists inside this
 * namespace.  Most of the macros begin with the string "SAWYER_". */
namespace Sawyer {

/** Explicitly initialize the library. This initializes any global objects provided by the library to users.  This happens
 *  automatically for many API calls, but sometimes needs to be called explicitly. Calling this after the library has already
 *  been initialized does nothing. The function always returns true. */
bool initializeLibrary();

/** True if the library has been initialized. */
extern bool isInitialized;

} // namespace

// Microsoft compilers are deficient in some respects
#ifdef _MSC_VER
# define SAWYER_ATTR_UNUSED /*unused*/
# define SAWYER_ATTR_NORETURN /*noreturn*/
#else
# define SAWYER_ATTR_UNUSED __attribute__((unused))
# define SAWYER_ATTR_NORETURN __attribute__((noreturn))
#endif

#endif
