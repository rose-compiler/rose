static void foo (void) {
#define EXPECTED_VIEW "/* The first line of this file must match the expectation of"
#define EXPECTED_VIEW_LENGTH (sizeof (EXPECTED_VIEW) - 1)
  if (EXPECTED_VIEW_LENGTH) {}
}

