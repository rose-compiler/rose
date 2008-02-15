//  ./am-utils-6.0.9-2/fsinfo-S3kd.i

// technically this should only work with this 'extern' here, but gcc
// allows it without; maybe make a copy in in/gnu that doesn't have
// the extern.

extern                          // optional in gnu
char hostname[];
char hostname[64 + 1];
int main() {
  sizeof(hostname);
}
