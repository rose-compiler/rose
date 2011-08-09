#include "InnerInternal1.h"
v4_rename_me = 55;
for (int i = 0; i < 10; i++) {
  if (i > 5) {
    #include "InnerInternal2.h"
    v5_rename_me = 11;
  } else {
    #include "InnerInternal3.h"
    v6_rename_me = 22;
  }
}
#include "InnerInternal4.h"
v7_rename_me = 33;
int v3_rename_me;
