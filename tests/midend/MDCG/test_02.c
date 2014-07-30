
#include "test_02.h"

struct latice_desc_t_ my_latice_0_desc = {
  "l0",
  {0, 0, 0}
};

struct latice_t_ my_latice_0 = {
  &my_latice_0_desc,
  5,
  {
    {"l0p0", {0,0,0}},
    {"l0p1", {0,0,0}},
    {"l0p2", {0,0,0}},
    {"l0p3", {0,0,0}},
    {"l0p4", {0,0,0}}
  }
};

struct latice_desc_t_ my_latice_1_desc = {
  "l1",
  {0, 0, 0}
};

struct latice_t_ my_latice_1 = {
  &my_latice_1_desc,
  3,
  {
    {"l0p0", {0,0,0}},
    {"l0p1", {0,0,0}},
    {"l0p2", {0,0,0}}
  }
};

struct latice_desc_t_ my_latice_2_desc = {
  "l2",
  {0, 0, 0}
};

struct latice_t_ my_latice_2 = {
  &my_latice_2_desc,
  6,
  {
    {"l0p0", {0,0,0}},
    {"l0p1", {0,0,0}},
    {"l0p2", {0,0,0}},
    {"l0p3", {0,0,0}},
    {"l0p4", {0,0,0}},
    {"l0p5", {0,0,0}}
  }
};

latice_t my_latices[3] = {
  &my_latice_0,
  &my_latice_1,
  &my_latice_2
};

struct world_t_ my_world = {
  3,
  my_latices
};
