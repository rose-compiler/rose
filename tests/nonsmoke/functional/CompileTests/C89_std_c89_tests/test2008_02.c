/* JJW tests for ifs without else clauses, including dangling else cases */

int main(int argc, char** argv) {
  int x, y, z;
  /* Test 1: basic, with no elses at all */
  if (x == 1)
    if (y == 2)
      z = 1;
  /* Test 2: else on inner if, no adjustment needed */
  if (x == 1)
    if (y == 2)
      z = 2;
    else
      z = 3;
  /* Test 3: else on both ifs, no adjustment needed */
  if (x == 1)
    if (y == 2)
      z = 4;
    else
      z = 5;
  else
    z = 6;
  /* Test 4: dangling else is possible if code is not unparsed correctly */
  if (x == 1)
    if (y == 2)
      z = 7;
    else;
  else
    z = 8;
  /* Test 5: an inner while loop */
  if (x == 1)
    while (y == 2)
      if (z == 3)
        z = 9;
      else;
  else
    z = 10;
  /* Test 6: multiple levels of empty elses */
  if (x == 1)
    if (y == 2)
      if (z == 3)
        z = 11;
      else;
    else;
  else
    z = 12;
  /* End of tests */
  return 0;
}
