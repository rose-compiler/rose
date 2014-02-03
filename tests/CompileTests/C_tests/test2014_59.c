#define ROSE_USE_INCLUDES 0

#define MSGSTRFIELD(line) MSGSTRFIELD1(line)
#define MSGSTRFIELD1(line) str##line


// DQ: Note that this will build the names of variables (data members) based on the line number.
static const struct msgstr_t {
#define WRAPPER(a,b) char MSGSTRFIELD(__LINE__) [sizeof (b)];

#if ROSE_USE_INCLUDES
#include "wrapper-types.h"
#else
char str6 [sizeof ("none")];
char str7 [sizeof ("delegate-invoke")];
char str8 [sizeof ("delegate-begin-invoke")];
char str9 [sizeof ("delegate-end-invoke")];
char str10 [sizeof ("runtime-invoke")];
char str11 [sizeof ("native-to-managed")];
char str12 [sizeof ("managed-to-native")];
char str13 [sizeof ("managed-to-managed")];
char str14 [sizeof ("remoting-invoke")];
char str15 [sizeof ("remoting-invoke-with-check")];
char str16 [sizeof ("xdomain-invoke")];
char str17 [sizeof ("xdomain-dispatch")];
char str18 [sizeof ("ldfld")];
char str19 [sizeof ("stfld")];
char str20 [sizeof ("ldfld-remote")];
char str21 [sizeof ("stfld-remote")];
char str22 [sizeof ("synchronized")];
char str23 [sizeof ("dynamic-method")];
char str24 [sizeof ("isinst")];
char str25 [sizeof ("castclass")];
char str26 [sizeof ("proxy_isinst")];
char str27 [sizeof ("stelemref")];
char str28 [sizeof ("unbox")];
char str29 [sizeof ("ldflda")];
char str30 [sizeof ("write-barrier")];
char str31 [sizeof ("unknown")];
char str32 [sizeof ("cominterop-invoke")];
char str33 [sizeof ("cominterop")];
char str34 [sizeof ("alloc")];
#endif
#undef WRAPPER
} opstr = {
#define WRAPPER(a,b) b,
#if ROSE_USE_INCLUDES
#include "wrapper-types.h"
#else
"none",
"delegate-invoke",
"delegate-begin-invoke",
"delegate-end-invoke",
"runtime-invoke",
"native-to-managed",
"managed-to-native",
"managed-to-managed",
"remoting-invoke",
"remoting-invoke-with-check",
"xdomain-invoke",
"xdomain-dispatch",
"ldfld",
"stfld",
"ldfld-remote",
"stfld-remote",
"synchronized",
"dynamic-method",
"isinst",
"castclass",
"proxy_isinst",
"stelemref",
"unbox",
"ldflda",
"write-barrier",
"unknown",
"cominterop-invoke",
"cominterop",
"alloc",
#endif
#undef WRAPPER
};


// DQ: Note that this will reference the names of variables based on the line number.
static const gint16 opidx [] = {
#define WRAPPER(a,b) [MONO_WRAPPER_ ## a] = offsetof (struct msgstr_t, MSGSTRFIELD(__LINE__)),
#if ROSE_USE_INCLUDES
#include "wrapper-types.h"
#else

// DQ: I had to preprocess the original file to get the source code that would have matching line numbers!

[MONO_WRAPPER_NONE] = __builtin_offsetof (struct msgstr_t, str6),
[MONO_WRAPPER_DELEGATE_INVOKE] = __builtin_offsetof (struct msgstr_t, str7),
[MONO_WRAPPER_DELEGATE_BEGIN_INVOKE] = __builtin_offsetof (struct msgstr_t, str8),
[MONO_WRAPPER_DELEGATE_END_INVOKE] = __builtin_offsetof (struct msgstr_t, str9),
[MONO_WRAPPER_RUNTIME_INVOKE] = __builtin_offsetof (struct msgstr_t, str10),
[MONO_WRAPPER_NATIVE_TO_MANAGED] = __builtin_offsetof (struct msgstr_t, str11),
[MONO_WRAPPER_MANAGED_TO_NATIVE] = __builtin_offsetof (struct msgstr_t, str12),
[MONO_WRAPPER_MANAGED_TO_MANAGED] = __builtin_offsetof (struct msgstr_t, str13),
[MONO_WRAPPER_REMOTING_INVOKE] = __builtin_offsetof (struct msgstr_t, str14),
[MONO_WRAPPER_REMOTING_INVOKE_WITH_CHECK] = __builtin_offsetof (struct msgstr_t, str15),
[MONO_WRAPPER_XDOMAIN_INVOKE] = __builtin_offsetof (struct msgstr_t, str16),
[MONO_WRAPPER_XDOMAIN_DISPATCH] = __builtin_offsetof (struct msgstr_t, str17),
[MONO_WRAPPER_LDFLD] = __builtin_offsetof (struct msgstr_t, str18),
[MONO_WRAPPER_STFLD] = __builtin_offsetof (struct msgstr_t, str19),
[MONO_WRAPPER_LDFLD_REMOTE] = __builtin_offsetof (struct msgstr_t, str20),
[MONO_WRAPPER_STFLD_REMOTE] = __builtin_offsetof (struct msgstr_t, str21),
[MONO_WRAPPER_SYNCHRONIZED] = __builtin_offsetof (struct msgstr_t, str22),
[MONO_WRAPPER_DYNAMIC_METHOD] = __builtin_offsetof (struct msgstr_t, str23),
[MONO_WRAPPER_ISINST] = __builtin_offsetof (struct msgstr_t, str24),
[MONO_WRAPPER_CASTCLASS] = __builtin_offsetof (struct msgstr_t, str25),
[MONO_WRAPPER_PROXY_ISINST] = __builtin_offsetof (struct msgstr_t, str26),
[MONO_WRAPPER_STELEMREF] = __builtin_offsetof (struct msgstr_t, str27),
[MONO_WRAPPER_UNBOX] = __builtin_offsetof (struct msgstr_t, str28),
[MONO_WRAPPER_LDFLDA] = __builtin_offsetof (struct msgstr_t, str29),
[MONO_WRAPPER_WRITE_BARRIER] = __builtin_offsetof (struct msgstr_t, str30),
[MONO_WRAPPER_UNKNOWN] = __builtin_offsetof (struct msgstr_t, str31),
[MONO_WRAPPER_COMINTEROP_INVOKE] = __builtin_offsetof (struct msgstr_t, str32),
[MONO_WRAPPER_COMINTEROP] = __builtin_offsetof (struct msgstr_t, str33),
[MONO_WRAPPER_ALLOC] = __builtin_offsetof (struct msgstr_t, str34),

#endif

#undef WRAPPER
};
