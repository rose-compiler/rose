#!/usr/bin/awk
{
  for (i = 1; i <= NF; i++) {
      printf("%c", $i+64)
  }
  printf("\n")
}
