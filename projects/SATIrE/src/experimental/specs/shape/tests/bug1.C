typedef struct {
	int intVectorDummy;
} IntVectorHandle;

typedef int IntVector;

void foo(IntVector* x) {
}

void intVectorHandleReporting (IntVectorHandle *intVectorHandle) {
  IntVector *intVector;
  foo(intVector = (IntVector *) intVectorHandle);
}

int main() {
  return 0;
}
