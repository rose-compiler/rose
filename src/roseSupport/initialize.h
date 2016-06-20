#ifndef ROSE_initialize_H
#define ROSE_initialize_H

/** Main namespace for the ROSE library. */
namespace rose {

/** Initialize the library.
 *
 *  This function serves two purposes:
 *
 *  @li Certain dependent libraries have initialization functions and that need to be invoked before any of their other
 *      functions are called. Rather than (re)invoke their initializer every time one of those functions is called, we can move
 *      their initializers into this one ROSE initializer. This also ensures that dependent libraries are initialized in the
 *      correct order since C++ provides no standard way to do this.
 *
 *  @li If the caller of ROSE functions (e.g., the user's program that uses the ROSE library) is compiled with one version of
 *      ROSE header files but the ROSE library was compiled with a different version this can result in runtime errors that are
 *      very difficult to track down.  The same is true for any other C++ library that has functions whose ABI includes passing
 *      copies of or references to objects.  Therefore, this initializer takes an argument which is a compile-time token from
 *      header files used by the user and compares it with the token compiled into the library.
 *
 *  This function should be called before any other ROSE library functions, although it is called implicitly by some common
 *  functions like @ref frontend.  It can be called more than once--subsequent calls do nothing.  If called concurrently by
 *  more than one thread, then one thread performs the initialization while the others block until it completes.
 *
 *  The @p configToken argument is normally the C preprocessor macro @c ROSE_CONFIG_TOKEN, which should be treated as
 *  a black box.  Passing a null pointer bypasses the configuration and version checking but still initializes the ROSE library
 *  and its dependencies.  If the configuration and version checking (see @ref checkConfigToken) detects an inconsistency then
 *  an error message is emitted and a <code>std::runtime_error</code> is thrown.
 *
 *  Example usage:
 *  @code
 *   #include <rose.h>
 *
 *   int main(int argc, char *argv[]) {
 *       ROSE_INITIALIZE; // or rose::initialize(ROSE_CONFIG_TOKEN);
 *  @endcode
 *
 *  <b>ROSE developers:</b> If you want to have @ref initialize (specifically, @ref checkConfigToken) check for consistency in
 *  some configuration property then modify the autoconf files ($ROSE/config directory) or CMakeLists.txt files to append to
 *  the @c ROSE_CONFIG_TOKEN shell variable or CMake variable, respectively.  This variable holds the string that will eventually
 *  become the expansion value of the @c ROSE_CONFIG_TOKEN C preprocessor macro.  There is no need for the autoconf string to be
 *  character-for-character identical with the cmake string, although they should check for the same kinds of things just for
 *  consistency.
 *
 *  See also, @ref isInitialized and @ref checkConfigToken. The @c ROSE_INITIALIZE C preprocessor-defined symbol expands to
 *  <code>rose::initialize(ROSE_CONFIG_TOKEN)</code> but might be easier to remember. */
void initialize(const char *configToken);

/** Checks whether the library has been initialized.
 *
 *  Returns true if the library initialization step has been completed, false if it hasn't started. If this is called when some
 *  other thread is initializing the library it blocks until the initialization has been completed and then returns true. Note
 *  that the notion of "when some other thread is initializing the library" is defined as beginning when that other thread
 *  enters the initialization critical section and ends when it leaves the critical section, which doesn't coincide exactly
 *  with the call to and return from @ref initialize. */
bool isInitialized();

/** Check the configuration token.
 *
 *  This function checks the supplied configuration token against the token compiled into the library and returns true if they
 *  match and false if they don't.  See @ref initialize. */
bool checkConfigToken(const char *configTokenToken);

} // namespace


// A slightly more memorable way to initialize ROSE
#define ROSE_INITIALIZE rose::initialize(ROSE_CONFIG_TOKEN)

#endif
