typedef unsigned long size_t;

void *operator new(size_t n);
void *operator new(size_t n, void *p);
void *operator new(size_t n, void *heap, size_t heap_size);
void *operator new(size_t n, size_t sz, void *p);

void *operator new[](size_t n);
void *operator new[](size_t n, size_t sz, void *p);

void foobar()
   {
     typedef float (*t)();
     t tbuf[10];
     t *pt = new (tbuf, sizeof(tbuf)) (t);
     float (**ppf)() = new (tbuf, sizeof(tbuf)) (float (*)());
     unsigned long int ulibuf[10];
   }

