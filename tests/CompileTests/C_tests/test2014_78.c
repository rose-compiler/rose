typedef unsigned long guint;
typedef unsigned int guint32;

int dirty;

// #define MAKE_NUMERIC_VERSION(a, b, c, d) ((((guint32)a) << 24) | (((guint32)b) << 16) | (((guint32)c) <<  8) |  ((guint32)d)      )
// #define MAKE_NUMERIC_VERSION(a) (((guint32)a) << 24)
#define MAKE_NUMERIC_VERSION(a) ((guint32)a)

// #define VERSION_NUMERIC		MAKE_NUMERIC_VERSION(601cff-dirty, 601cff-dirty, 601cff, 0)
#define VERSION_NUMERIC MAKE_NUMERIC_VERSION(0x601cff-dirty)

// #define VERSION_GIT_FULL	"601cff-dirty"


guint claws_get_version(void)
   {
     return VERSION_NUMERIC;
      
   }

