int x;

typedef int MyInt;
typedef double MyDouble;

namespace Y { int x; int moo(); }

namespace Z { using namespace Y; int z; }

class B { public: int b; private: int a;};

class A : B {

        public:

                int a;

                int foo() {
                        int x = a;
                        int y;
                        return x;
                }

                int y;

                int goo();

        private:

                int b;

};

int A::goo() {

        int a, b;

        b = 1;

        int wwwwwwwwwwwwww;

        return 0;

}

using namespace Y;

int main() {

        using Z::x;

        {

                int x, y;
                float moo;
                double MyInt;

        }

        return 0;

}

/* proofed output from HiddenList program on April 16, 2007 under ROSE Version: pre-release alpha version: 0.8.10a

Inside of Sg_File_Info::display(file: debug)
     isTransformation                      = false
     isCompilerGenerated                   = false
     isOutputInCodeGeneration              = false
     isShared                              = false
     isFrontendSpecific                    = false
     isSourcePositionUnavailableInFrontend = false
     isCommentOrDirective                  = false
     isToken                               = false
     filename = /home/preissl1/project/ROSE_MARCH31/test04.C
     line     = 1  column = 1
     file_id  = 0
     filename = /home/preissl1/project/ROSE_MARCH31/test04.C
     line     = 1  column   = 1
Hidden List of Scope: ::Y
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
 -> Number of entries in hidden list: 1
 
Hidden List of Scope: ::A
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: b with adress(es) : Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d140
 -> Number of entries in hidden list: 1
 
Hidden List of Scope:
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
Symbol: y with adress(es) : Hidden entry y; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d128
 -> Number of entries in hidden list: 2
 
Hidden List of Scope:
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: a with adress(es) : Hidden entry a; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d0e0
Symbol: b with adress(es) : Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d0b0
Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d140
 -> Number of entries in hidden list: 3
 
Hidden List of Scope:
NR. OF DIFFERENT HIDDEN SYMBOLS: 3
Symbol: moo with adress(es) : Hidden entry moo; with SgSymbol-ClassName SgFunctionSymbol with SgSymbol-Address: 0x99eab98
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d080
Symbol: MyInt with adress(es) : Hidden entry MyInt; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x99a1a10
 -> Number of entries in hidden list: 4
*/


