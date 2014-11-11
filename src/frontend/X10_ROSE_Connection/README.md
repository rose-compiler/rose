# README For X10/Rose Connection (Michihiro Horie, 09/09/14)


## X10
X10 is a class-based, strongly-typed, garbage-collected, object-oriented language. To support concurrency and distribution, X10 uses the Asynchronous Partitioned Global Address Space programming model (APGAS). Both its modern, type-safe sequential core and simple programming model for concurrency and distribution contribute to making X10 a high-productivity language in the HPC and Big Data spaces. 

* Official site is [here!](http://x10-lang.org/)
* You can install X10 easily even if you do not have one. See the next section "How to set up ROSE and X10".

## How to set up ROSE and X10

If you just cloned edg4x-rose and have not installed ROSE yet, it is a great chance to use our automatic installation scripts for ROSE and X10!!

In the same directory as this README (thus, edg4x-rose/src/frontend/X10\_ROSE\_Connection/), you can find two scripts named **setenv-rose.sh** and **setup-rose.sh**

### Copy the **setenv-rose.sh** and **setup-rose.sh** into a directory where you install ROSE and X10

>$cd /home/michihiro/   

>$mkdir x10rose

>$cp  set*-rose.sh x10rose

>$cd x10rose

>$export HOME=/home/michihiro/x10rose     # necessary components are installed directly under $HOME

>$chmod 644 setenv-rose.sh

>$chmod 755 setup-rose.sh


### Execute **setenv-rose.sh**

>$sh setenv-rose.sh


Setenv-rose.sh will set the following environment variables:

>export JAVA_HOME=$HOME/opt/jdk1.7.0_67               #Note that JAVA_HOME should point Oracle JDK 1.7.0
 
>_BOOST_ROOT=$HOME/opt/boost

>_ROSE_ROOT=$HOME/rose

>_X10_VER=2.4.3.2

>_X10_DIST=$HOME/x10/x10-$_X10_VER/x10.dist

You can change their definitions by modifying the script.

### Then execute **setup-rose.sh**

>$sh setup-rose.sh

Since you will be asked "(y/N)?" several times, please observe the progress for a while after running setup-rose.sh.

 Now you have ROSE and X10 installed!
 Note that it can take a couple of hours to finish depending on the machine you have.    
    


## How to run X10/ROSE Connection

First of all, you need a driver code to handle X10 file(s). For example, you can use **identityTranslator** that is located in rose/build/commit-*/exampleTranslators/documentedExamples/simpleTranslatorExamples/. It  invokes ROSE functions **frontend()** and **backend()** in its main(). (Source code identityTranslator.C is located in edg4x-rose/exampleTranslators/documentedExamples/simpleTranslatorExamples.)

The frontend() runs the X10 compiler, parses a polyglot AST that the X10 compiler creates, and copies the polyglot AST to an ROSE-based AST. The backend() generatesX10 source code from the ROSE AST.

### Example X10 code

You can find simple examples in /home/michihiro/x10rose/rose/edg4x-rose/src/frontend/X10\_ROSE_Connection/samples/.

### Try to run X10/Rose Connection!

>$cd rose/build/commit-*/exampleTranslators/documentedExamples/simpleTranslatorExamples

>$./identityTranslator Fibonacci.x10 >> log 2>> log        # Currently, many debugging info is printed

You can find unparsed Fibonacci.x10 in the **./rose-output**, which will be created after running identityTranslator.

>$cat rose-output/Fibonacci.x10

## Contact

####  For suggestions, questions and/or comments, please contact Michihiro Horie (horie@jp.ibm.com).


