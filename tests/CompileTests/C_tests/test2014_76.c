// We see in an application that both of these are decleared, but it is not
// clear how this could be legal code.  In the application the 2nd is commented
// out when using ROSE.  But we still don't understand the error in the context
// of the applicaiton.

enum
  {
    DT_UNKNOWN = 0,
    DT_FIFO = 1,
    DT_CHR = 2,
    DT_DIR = 4,
    DT_BLK = 6,
    DT_REG = 8,
    DT_LNK = 10,
    DT_SOCK = 12,
    DT_WHT = 14
  };

#if 0
// This can't be declared if the previous un-named enum was declared (using the same enum values).
enum gluster_entry_types {
 DT_UNKNOWN = 0,
 DT_FIFO = 1,
 DT_CHR = 2,
 DT_DIR = 4,
 DT_BLK = 6,
 DT_REG = 8,
 DT_LNK = 10,
 DT_SOCK = 12,
 DT_WHT = 14
};
#endif


