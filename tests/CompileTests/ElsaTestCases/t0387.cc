// t0387.cc
// overload resolution getting same function twice?

class MyEmitCode {};


template <class EltType>
void emitTable(MyEmitCode &out, EltType const *table, int size, int rowLength,
               char const *typeName, char const *tableName)
{
}

 
template <class EltType>
void emitOffsetTable(MyEmitCode &out, EltType **table, EltType *base, int size,
                     char const *typeName, char const *tableName, char const *baseName)
{
  char const *s;
  emitTable(out, (int*)0, size, 16, "int", s);
}

void ParseTables__emitConstructionCode(MyEmitCode &out,
  char const *className, char const *funcName)
{
  unsigned char **errorBitsPointers;
  unsigned char *errorBits;
  int numStates;
  emitOffsetTable(out, errorBitsPointers, errorBits, numStates,
                  "ErrorBitsEntry*", "errorBitsPointers", "errorBits");
}



