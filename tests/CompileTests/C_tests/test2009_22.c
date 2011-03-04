/*
bit fields

Contributed by Philippe Charles

Liao 2/12/2010
*/
typedef struct {
  unsigned offset          : 16;
  unsigned page           : 8;
  unsigned segment     : 6;
  unsigned unused       : 1;
  unsigned supervisor  : 1+3;
} VirtualAddress;
