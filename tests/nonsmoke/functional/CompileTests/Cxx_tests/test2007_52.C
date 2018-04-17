#if 0
I think you may already know about this one (and maybe even have it fixed), but the following (extracted from BusyBox):

struct A *f();
struct A {};

unparses with a name qualification on the first line:

extern struct ::A *f();

struct A
{
}

;

which is not correct.

-- Jeremiah Willcock 
#endif

// DQ (5/23/2007): This should be fixed by the newer name qualification mechanism, when it is finished.
struct A *f();
struct A {};

