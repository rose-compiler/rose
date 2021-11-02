#include <stdlib.h>

int main() {
  void* p=alloca(20);
}

// Reproduce issue with: codethorn RC-1096.c --normalize-level=2 --unparse
// shows wrong return type
