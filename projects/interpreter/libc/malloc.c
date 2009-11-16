#include <malloc.h>
#include <string.h>

void *calloc(size_t nmemb, size_t size)
   {
     void *region = malloc(nmemb*size);
     memset(region, 0, nmemb*size);
     return region;
   }
