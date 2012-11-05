typedef struct _GOptionEntry GOptionEntry;

typedef enum
{
  G_OPTION_ARG_NONE,
  G_OPTION_ARG_STRING,
} GOptionArg;

struct _GOptionEntry
   {
     const char *long_name;
     int flags;

     GOptionArg arg;

     const char *description;
   };

// The issue is that when this comes from a seperate file, values are not unparsed.
#include "test2012_156.h"

void foobar()
   {
     static GOptionEntry options[] = 
        {
        // This unparses as: {("session"), (G_OPTION_ARG_STRING), ("abcdefg")}};
           { "session", G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, "abcdefg" }
        };
   }
