// The forward declaration of an enum is allowed by many compilers and by EDG, but not by g++.
// it is also not part of the C or C++ standard (as I understand from the C and C++ statard directly
// and several discussions of the subject on the web.  It is a common C and C++ extension however,
// and a nice example of the liberties that many vendors have had with C and C++ even in this time of
// obsession with standards :-).
// enum enumTag;

// An enum must be defined before it can be used (it is in the C and C++ standard)
enum enumTag { a };

#if __cplusplus
enumTag* enumVarPtr;
#else
enum enumTag* enumVarPtr;
#endif

// A redundent reference to the enum is allowed, but is totally worthless. I don't know why 
// this is permitted, but it is not implemented in the EDG/Sage interface so this might be
// an issue in the future.  I will not bother fixing it now since I can't see how it could 
// be a problem.
enum enumTag;

