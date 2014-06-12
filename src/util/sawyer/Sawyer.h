#ifndef Sawyer_H
#define Sawyer_H

namespace Sawyer {

/** Explicitly initialize the library. This initializes any global objects provided by the library to users.  This happens
 *  automatically for many API calls, but sometimes needs to be called explicitly. Calling this after the library has already
 *  been initialized does nothing. The function always returns true. */
bool initializeLibrary();

/** True if the library has been initialized. */
extern bool isInitialized;

} // namespace

#endif
