#ifndef COLLECT_MODULE

#define COLLECT_MODULE

typedef void (*Collection_FuncPos)  (void *object, int pos);
typedef void (*Collection_Func)     (void *object);
typedef int  (*Collection_Comp)     (void *object, void *data);

typedef struct {
  int el_size;
  int n_ext;
  int size;
  int el_free;
  char *data;
} Collection;

typedef Collection *PCollection;

void Collection_Init(PCollection Col, int elem_s, int size, int extend);
void Collection_Done(PCollection Col);
void Collection_Clean(PCollection Col);
int  Collection_New(PCollection Col);
int  Collection_Count(PCollection Col);
void *Collection_At(PCollection  Col, int pos);
void Collection_Get(PCollection  Col, int pos, void *data);
void Collection_Put(PCollection  Col, int pos, void *data);
void Collection_ForEach(PCollection  Col, Collection_Func fn);
void Collection_ForEachPos(PCollection Col, Collection_FuncPos fn);
int  Collection_FirstThat(PCollection  Col, Collection_Comp fn, void *data);
#endif



