#ifndef SET_MODULE

#define SET_MODULE

typedef void (*Set_Func) (int object);

typedef struct {
  int size;
  unsigned short int *data;
} Set;

typedef Set * PSet;

#define SET_NBITS    16

void Set_Init(PSet set);
void Set_Done(PSet set);
void Set_Clean(PSet set);
void Set_AddItem(PSet set, int n);
void Set_DelItem(PSet set, int n);
int  Set_IsItem(PSet set, int n);
int  Set_MaxIndex(PSet set);
int  Set_MinIndex(PSet set);
void Set_GetRange(PSet set, int *s, int *f);
int  Set_Elements(PSet set);
int  Set_Empty(PSet set);
void Set_ForEach(PSet set, Set_Func fn);
void Set_Union(PSet set1, PSet set2);
void Set_Copy(PSet set1, PSet set2);
void Set_Diference(PSet set1, PSet set2);
void Set_Intersect(PSet set1, PSet set2);
void Set_AddRange(PSet set, int start, int end);
void Set_DelRange(PSet set, int start, int end);
int  Set_Equal(PSet set1, PSet set2);
int  Set_Diferent(PSet set1, PSet set2);
int  Set_Includes(PSet set, PSet set2);
void Set_PrintInt(PSet set);
void Set_PrintChar(PSet set);
#endif


