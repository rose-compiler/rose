
// Simplest example of bug similar to test2004_94.C
class X;
X* x = 0L;

// If this is commented out then the unparse generates
// class X {} *x = ((class X *)0);
// for the above declaration statement of x

class X {};

