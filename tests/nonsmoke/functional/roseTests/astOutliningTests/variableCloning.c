#include <stdio.h>
#include <stdlib.h>

const char *abc_soups[10] = {("minstrone"), ("french onion"), ("Texas chili"), ("clam chowder"), ("potato leek"), ("lentil"), ("white bean"), ("chicken noodle"), ("pho"), ("fish ball")};

int main (void)
{
// split variable declarations with their initializations, as a better demo for the outliner
  const char * soupName;
  int value; 
#pragma rose_outline
  {
    value = rand();
    soupName = abc_soups[value  % 10];
  }

  printf ("Here are your %d,  %s soup\n", value, soupName);
  return 0;
}

