// gcc accepts this in C mode, not in C++ mode (per the C++ spec)

typedef struct zone_struct {
  int x;
} zone_t;
struct zone_t;
