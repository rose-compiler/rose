
struct point_t_ {
  char * name;
  unsigned coords[3];
};
typedef struct point_t_ * point_t;

struct latice_desc_t_ {
  char * name;
  unsigned color[3];
};
typedef struct latice_desc_t_ * latice_desc_t;

struct latice_t_ {
  latice_desc_t descriptor;

  unsigned num_points;
  struct point_t_ points[];
};
typedef struct latice_t_ * latice_t;

struct world_t_ {
  unsigned num_latices;
  latice_t * latices;
};
typedef struct world_t_ * world_t;
