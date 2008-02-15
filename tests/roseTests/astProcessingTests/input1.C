int globalVariableX;  
#if 1


class A 
{
  public: int x;
  int foo();
}
;  

#endif
int GlobalScopePreambleStart;
int GlobalScopePreambleEnd;
int CurrentLocationTopOfScopeStart;
int CurrentLocationTopOfScopeEnd;
int CurrentLocationBeforeStart;
int CurrentLocationBeforeEnd;
int CurrentLocationReplaceStart;
// int main(){int x;int y;int z;x + 1;int i;for(i = 0;i < 5;i++) {x += i;y += i;z += i;}switch(x){case 1:{x++;break;}default:{x--;}}return 0;}
int main(){int x;int y;int z;x + 1;int i;for(i = 0;i < 5;i++){x += i;y += i;z += i;}return 0;}
int CurrentLocationReplaceEnd;
int CurrentLocationAfterStart;
int CurrentLocationAfterEnd;
int CurrentLocationBottomOfScopeStart;
int CurrentLocationBottomOfScopeEnd;

/* Reference marker variables to avoid compiler warnings */ 
void avoidCompilerWarnings() { 
GlobalScopePreambleStart++;
GlobalScopePreambleEnd++;
CurrentLocationTopOfScopeStart++;
CurrentLocationTopOfScopeEnd++;
CurrentLocationBeforeStart++;
CurrentLocationBeforeEnd++;
CurrentLocationReplaceStart++;
CurrentLocationReplaceEnd++;
CurrentLocationAfterStart++;
CurrentLocationAfterEnd++;
CurrentLocationBottomOfScopeStart++;
CurrentLocationBottomOfScopeEnd++;
}
template < class T > void foo(T i) {}

int main2()
   {
     int i = 42;

  // This forces foo(T) to be instantiated
     foo(i);
   }

