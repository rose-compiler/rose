# README For ROSE X10 Connection (Michihiro Horie, 04/21/14)

## FILES
* x10_main.cpp - defines a function to invoke x10 compiler (polyglot.main.Main class) 
* x10ActionROSE.C, ParserActionROSE.C - JNI native functions to create ROSE ASTs
* x10Parser.h - header file for x10ActionROSE.C
* Parser.h - header file for ParserActionROSE.C
* Other files 
    * Helper functions - jni_x10_utils.[Ch], x10_support.[Ch], jni_x10_token.[Ch], jni_x10SourceCodePosition.[Ch], x10SourceCodePosition.[Ch], jserver.[Ch], x10.[Ch], x10Traversal.[Ch]

## Current support for .x10 files (04/21/14)
* Supported "SgProject *frontend()" and "void generateDOT()" functions for .x10 files
* Supported still limited ROSE AST nodes. See x10ActionROSE.C for details. 


## How to run X10 ROSE Connection
* One of supported code is the serial fibonacci function:

1. Prepare Fib_serial.x10, which has code such as: 
     

    public class Fib_serial {  
        public static def fib(n:long) {  
            if (n < 2) return 1;  
            return fib(n-2) + fib(n-1);  
        }

        public static def main(args:Rail[String]) {
            var f:Long = fib(10);
        }
    }

2. Run sample code to invoke frontend() and createDOT() functions. For example, you can use sample code defined in  
edg4x-rose/exampleTranslators/documentedExamples/AstRewriteExamples/astRewriteExample1.C with the main() such as:  


    int main ( int argc, char** argv ) {  
        SgProject* sageProject = frontend(argc,argv);  
        generateDOT(*sageProject);  
        return 0;  
    }   
 
3. Make and run a sampel code  


    $ cd <ROSE Instalaltion directory>/edg4x-rose/installTree/exampleTranslators/documentedExamples/AstRewriteExamples/  
    $ make  
    $ ./astRewriteExample1 Fib_serial.x10   # you can get a .dot file to represent ROSE AST for Fib_serial.x10 
