# README For ROSE X10 Connection (Michihiro Horie, 09/09/14)

## FILES
* x10_main.cpp - defines a function to invoke x10 compiler (polyglot.main.Main class) 
* x10ActionROSE.C, ParserActionROSE.C - JNI native functions to create ROSE ASTs
* x10Parser.h - header file for x10ActionROSE.C
* Parser.h - header file for ParserActionROSE.C
* Other files 
    * Helper functions - jni_x10_utils.[Ch], x10_support.[Ch], jni_x10_token.[Ch], jni_x10SourceCodePosition.[Ch], x10SourceCodePosition.[Ch], jserver.[Ch], x10.[Ch], x10Traversal.[Ch]

## Current support for .x10 files (04/21/14)
* Supported parser and unparser for X10 code
* Supported still limited ROSE AST nodes. See x10ActionROSE.C for details. 


## How to run X10 ROSE Connection
* If you have not installed ROSE yet, see the next section "How to set up ROSE"
* This section explains how to use X10/Rose Connection with two examples:

1-1. Prepare Fib_parallel.x10, which has code such as: 
     
public class Fib_parallel {

    public static def fib(n:long) {
        if (n<2) return 1;
        val f1:long;
        val f2:long;
        finish {
          async { f1 = fib(n-1); }
          f2 = fib(n-2);
        }
        return f1 + f2;
    }

    public static def main(args:Rail[String]) {
        val f:long;
        f = fib(10);
      }
    }

1-2. Run sample code to invoke frontend() and backend() functions. For example, you can use sample code defined in  
edg4x-rose/exampleTranslators/documentedExamples/AstRewriteExamples/astRewriteExample1.C with the main() such as:  


    int main ( int argc, char** argv ) {  
        SgProject* sageProject = frontend(argc,argv);  // parse X10 code
        generateDOT(*sageProject);  // generate a .dot file 
		return backend(sageProject);  // unparse X10 code
    }   
 
1-3. Make and run a sampel code  


    $ cd <ROSE Instalaltion directory>/edg4x-rose/installTree/exampleTranslators/documentedExamples/AstRewriteExamples/  
    $ make  
    $ ./astRewriteExample1 Fib_parallel.x10   # you can get a .dot file to represent ROSE AST for Fib_serial.x10 


1-4. You will get unparsed X10 code

    public class Fib_parallel {
        public static def fib(n : long) : long {
        {
            if (n < 2L) {
                return 1L;
            }
            val f1 : long;
            val f2 : long;
            finish {
                async {
                    f1 = fib(n - 1L);
                }
                f2 = fib(n - 2L);
            }
            return f1 + f2;
        }
		}

        public static def main(args : Rail[x10.lang.String])  {
        {
            val f : long;
            f = fib(10L);
        }
        }
    
        /*
        final public def Fib_parallel$$this$Fib_parallel() : Fib_parallel {
        {
            return this;
        }
        }
        */

        public def this() : Fib_parallel{
        {
             ;
             ;
        }
        }
    }

2-1. Prepare QSort.x10, which has code such as: 

    public class QSort {
        private static def partition(data:Rail[int], left:long, right:long) {
        var i:long = left;
        var j:long = right;
        var tmp:int;
        var pivot:long = data((left + right) / 2);
        while (i <= j) {
            while (data(i) < pivot) i++;
            while (data(j) > pivot) j--;
            if (i <= j) {
                tmp = data(i);
                data(i) = data(j);
                data(j) = tmp;
                i++;  j--;
            }
        }
        return i;
    }

    static def qsort(data:Rail[int], left:long, right:long) {
        index:long = partition(data, left, right);
        finish { if (left < index - 1)  async qsort(data, left, index - 1);
                   if (index < right)     qsort(data, index, right);
        }
    }

    public static def main(args:Rail[String]) {
        val N = args.size>0 ? Long.parse(args(0)) : 100;
        val data = new Rail[int](N);
        qsort(data, 0, N-1);
    }

2-2. (Same as 1-2.) Run sample code to invoke frontend() and backend() functions. For example, you can use sample code defined in  
edg4x-rose/exampleTranslators/documentedExamples/AstRewriteExamples/astRewriteExample1.C with the main() such as:  
 
2-3. (Same as 1-3) Make and run a sampel code  

2-4. You will get unparsed X10 code
    public class QSort {
        private static def partition(data : Rail[int], left : long, right : long) : long {
        {
            var i : long = left;
            var j : long = right;
            var tmp : int;
            var pivot : long = (data((left + right) / 2L)) as long;
            while (i <= j) {
                while ((data(i)) as long < pivot) i++;
                while ((data(j)) as long > pivot) j--;
                if (i <= j) {
                    tmp = data(i);
                    data(i) = data(j);
                    data(j) = tmp;
                    i++;
                    j--;
                }
            }
            return i;
        }
        }

        static def qsort(data : Rail[int], left : long, right : long)  {
        {
            val index : long = QSort.partition(data,left,right);
            finish {
                if (left < index - 1L) {
                    async {
                        QSort.qsort(data,left,index - 1L);
                    }
                }
                if (index < right) {
                    QSort.qsort(data,index,right);
                }
            }
        }
        }

        public static def main(args : Rail[x10.lang.String])  {
        {
            val N : long = args.size > 0L ? Long.parse(args(0))  : 100L;
            val data : Rail[int] = new Rail[int](N);
            QSort.qsort(data,0L,N - 1L);
        }
        }

        // Comment out. Dees not support the constrainted type so far.
        /*
        final public def QSort$$this$QSort() : QSort {
        {
            return this;
        }
        }
        */

        public def this() : QSort{
        {
            ;
            ;
        }
        }
    }

## How to set up ROSE
* In edg4x-rose/src/frontend/X10_ROSE_Connection/, you can find two scripts named setenv-rose.sh and setup-rose.sh
* Note that it can take a couple of hours to finish depending on the machine you have


1. Save the attached setenv-rose.sh and setup-rose.sh files in the same directory.

$ chmod 644 setenv-rose.sh
$ chmod 755 setup-rose.sh

2. execute ./setenv-rose.sh 

setenv-rose.sh script will set the following envionment variables.  You can change their definitions by modifying the script.

export JAVA_HOME=$HOME/opt/jdk1.7.0_67		Note that JAVA_HOME should point Oracle JDK 1.7.0
_BOOST_ROOT=$HOME/opt/boost
_ROSE_ROOT=$HOME/rose
_X10_VER=2.4.3.2
_X10_DIST=$HOME/x10/x10-$_X10_VER/x10.dist


2. Then execute ./setup-rose.sh

3. Now you have ROSE installed!
