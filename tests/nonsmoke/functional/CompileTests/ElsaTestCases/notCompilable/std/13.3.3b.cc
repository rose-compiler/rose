// 13.3.3b.cc

void Fcn(const int*,  short);        // line 3
void Fcn(int*, int);                 // line 4

int i;
short s = 0;

void f() {                           // line 9
  // turn on overload resolution
  __testOverload(f(), 9);

  //ERROR(1): Fcn(&i, s);            // is ambiguous because
                                     // &i->int* is better than &i->const int*
                                     // but s->short is also better than s->int

  __testOverload(Fcn(&i, 1L), 4);    // calls Fcn(int*, int), because
                                     // &i->int* is better than &i->const int*
                                     // and 1L->short and 1L->int are indistinguishable

  __testOverload(Fcn(&i,'c'), 4);    // calls Fcn(int*, int), because
                                     // &i->int* is better than &i->const int*
                                     // and c->int is better than c->short
}
