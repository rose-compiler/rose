#include <stdio.h>
#include <stdlib.h>

const char *abc_soups[10] = {("minstrone"), ("french onion"), ("Texas chili"), ("clam chowder"), ("potato leek"), ("lentil"), ("white bean"), ("chicken noodle"), ("pho"), ("fish ball")};

int main (void)
{
// split variable declarations with their initializations, as a better demo for the outliner
  const char *soupName;
#pragma rose_outline
  {
    soupName = abc_soups[rand () % 10];
  }

  printf ("Here are your %s soup\n",  soupName);
  return 0;
}

