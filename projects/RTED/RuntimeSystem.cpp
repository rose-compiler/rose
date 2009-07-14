/*
 * RuntimeSystem.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: panas2
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

//#include "RuntimeSystem.h"
#include "CppRuntimeSystem/CppRuntimeSystem.h"
#include "rted_qt/rted_qt.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int rs_initialized = 0;
/*********************************************************
 * Constructor
 ********************************************************/
RuntimeSystem* 
RuntimeSystem_getRuntimeSystem() {
  RuntimeSystem * rs = RuntimeSystem::instance();

  if( 0 == rs_initialized) {
    rs->registerOutputStream( &cerr);
    rs_initialized = 1;
  }

  return rs;
}

/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void
RuntimeSystem_roseRtedClose() {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->doProgramExitChecks();

  // The runtime system would have exited if it had found an error
  rs->log( "Failed to discover error in RTED test\n" );
  exit( 1);
}


// ***************************************** ARRAY FUNCTIONS *************************************


bool checkAddress(unsigned long int address) {
  bool valid=true;
	if (address==66) {
	  // fixme: cannot handle right now!!
	  // we can not handle structs and classes right now
	  valid=false;
	}
	return valid;
}


/*********************************************************
 * This function is called when an array is created
 * name      : variable name
 * manglname : variable mangl_name
 * type      : Sage Type
 * dimension : 1 or 2
 * sizeA     : size of dimension 1
 * sizeB     : size of dimension 2
 * ismalloc  : Allocated through malloc?
 * filename  : file location
 * line      : linenumber
 ********************************************************/
void
RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name, int dimension,  
				   const char* type, const char* basetype,
			      unsigned long int address, long int size, long int sizeA, long int sizeB,
			      int ismalloc, const char* filename, const char* line, const char* lineTransformed){


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));

	   if (!checkAddress(address)) return; 
  if( 0 == strcmp("SgArrayType", type)) 
    if (dimension==1)
      rs->createMemory(address, sizeA);
    else
      rs->createMemory(address, sizeA*sizeB);
  else if( 0 == strcmp("SgPointerType", type))
    // address refers to the address of the variable (i.e. the pointer).
    // we want the address of the newly allocated memory on the heap
    if (dimension==1)
      rs->createMemory(  *((addr_type*)address), sizeA);
    else
      rs->createMemory(  *((addr_type*)address), sizeA*sizeB);
  else {
    cerr << "Unexpected Array Type: " << type << endl;
    exit( 1);
  }

}

/*********************************************************
 * This function is called when an array is accessed
 * name      : variable name
 * posA      : array[posA][]
 * posB      : array[][posB]
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_roseArrayAccess(const char* name, int posA, int posB, const char* filename,
			      unsigned long int address, long int size, 
			      const char* line, const char* lineTransformed, const char* stmtStr){


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	//fixme: we do not know yet wheather array access
	//is a read or write
	bool read = true;
	if (!checkAddress(address)) return; 
	if (read)
	  rs->checkMemRead(address,size);

}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************


/*********************************************************
 * This function is called when a variable is put or dropped from stack
 * stack variables are used to keep track of what variables are passed
 * to functions. Their mangled_names help to identify the real definitions.
 * name         : variable name if it is a variable to be put on the stack
 * mangled_name : mangled name of the above
 * insertBefore : Indicates whether to push or pop a variable form the stack
 ********************************************************/
void
RuntimeSystem_roseCallStack(const char* name, const char* mangl_name,
			    const char* beforeStr,
			    const char* filename, const char* line) {
}



/*********************************************************
 * Check if a function call is interesting, i.e. contains a
 * call to a function that we need to check the parameters of
 ********************************************************/
int
RuntimeSystem_isInterestingFunctionCall(const char* name) {
  int interesting=0;//false;
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strcpy")==0 ||
	 strcmp(name ,"strncpy")==0 ||
	 strcmp(name ,"strcat")==0 ||
	 strcmp(name ,"strncat")==0 ||
	 strcmp(name ,"strlen")==0 ||
	 strcmp(name ,"strchr")==0 ||
	 strcmp(name ,"strpbrk")==0 ||
	 strcmp(name ,"strspn")==0  ||
	 strcmp(name ,"strstr")==0
	 )) {
    interesting=1;
  }
  return interesting;
}

/*********************************************************
 * Check if the function that is called is modifying
 * i.e. whether it changes memory or just accesses it.
 * If it accesses memory, we need to ensure that the
 * memory is NULL terminated if const char*
 * we assume that parameter 2 than has the \0 token
 ********************************************************/
int
RuntimeSystem_isModifyingOp(const char* name) {
  int modifying=0;//false;
  // we do not need the actualLength of the
  // first parameter for the following
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strcpy")==0 ||
	 strcmp(name ,"strncpy")==0
	 )) {
    modifying=1;
  }
  return modifying;
}

/*********************************************************
 * Return the number of parameters for a specific function call
 ********************************************************/
int
RuntimeSystem_getParamtersForFuncCall(const char* name) {
  int dim=0;
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strncat")==0 )) {
    dim=3;
  } else if ((
	      strcmp(name ,"strncpy")==0 ||
	      strcmp(name ,"strcpy")==0 ||
	      strcmp(name ,"strchr")==0 ||
	      strcmp(name ,"strpbrk")==0 ||
	      strcmp(name ,"strspn")==0  ||
	      strcmp(name ,"strstr")==0  ||
	      strcmp(name ,"strcat")==0 )) {
    dim=2;
  } else if ((
	      strcmp(name ,"strlen")==0
	      )) {
    dim=1;
  }
  return dim;
}

/*********************************************************
 * Check if a function call is a call to a function
 * on our ignore list. We do not want to check those
 * functions right now.
 * This check makes sure that we dont push variables
 * on the stack for functions that we dont check
 * and hence the generated code is cleaner
 ********************************************************/
int
RuntimeSystem_isFileIOFunctionCall(const char* name) {
  int interesting=0;//false;
  if ( ( strcmp(name,"fopen")==0 ||
	 strcmp(name ,"fgetc")==0 ||
	 strcmp(name ,"fputc")==0 ||
	 strcmp(name ,"fclose")==0
	 )) {
    interesting=1;
  }
  return interesting;
}




// This handles string constants.  The present instrumentation does not
// transform these in any way, so the memory manager doesn't know that it's
// legal to read from them, since they're initialized at load time.
//
// A general approach to solving this problem would be to update the
// instrumentation (e.g. by making string constants globals, or otherwise
// registering them during the preamble inserted into main), but in the
// meantime, simply treating them as "lazily initialized" variables is
// sufficient for the memory manager to recognize that reads are valid.
//
// This does, however, mean that the memory manager will also think writes are
// valid, as in, e.g.
//  
//    char* s = malloc( 5);
//    strcpy( s, "1234";
//    strcpy( "a constant string", s);
#define HANDLE_STRING_CONSTANT( i )                                           \
  if( isdigit(  args[ i + 1 ][0]))                                            \
    RuntimeSystem_ensure_allocated_and_initialized(                           \
        args[ i ], strtol( args[ i + 1 ], NULL, 10)                           \
    );
void RuntimeSystem_ensure_allocated_and_initialized( const void* mem, size_t size) {
	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

  // We trust that anything allocated is properly initialized -- we're not
  // working around a string constant so there's no need for us to do anything.
  if( NULL != rs->getMemManager()->findContainingMem( (addr_type) mem))
    return;

  rs->createVariable(
      (addr_type) mem,
      "StringConstant",
      "MangledStringConstant",
      "SgArrayType",
      size
  );
  rs->createMemory( (addr_type) mem, size);
  rs->checkMemWrite( (addr_type) mem, size);
}

#define NUM_ARG( i )                                                          \
  strtol(                                                                     \
    isdigit( args[ i + 1 ][ 0 ]) ? args[ i + 1 ] : args[ i ],                 \
    NULL,                                                                     \
    10                                                                        \
  )
    

/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * memcpy, memmove, strcpy, strncpy, strcat, strncat
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_handleSpecialFunctionCalls(const char* fname,const char** args, int argsSize,
					 const char* filename, const char* line,
					 const char* lineTransformed,
					 const char* stmtStr, const char* leftHandSideVar) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));

  // FIXME 2: The current transformation outsputs num (for, e.g. strncat) as
  //    (expr), (size in str)
  // but size in str is off by one -- it includes the null terminator, but
  // should not.

  if( 0 == strcmp("memcpy", fname)) {
    rs->check_memcpy(
      (void*) args[0], 
      (const void*) args[2], 
      NUM_ARG( 4)
    );
  } else if ( 0 == strcmp("memmove", fname)) {
    rs->check_memmove(
      (void*) args[0], 
      (const void*) args[2], 
      NUM_ARG( 4)
    );
  } else if ( 0 == strcmp("strcpy", fname)) {
    HANDLE_STRING_CONSTANT( 0);
    HANDLE_STRING_CONSTANT( 2);

    rs->check_strcpy(
      (char*) args[0],
      (const char*) args[2]
    );
  } else if ( 0 == strcmp("strncpy", fname)) {
    rs->check_strncpy(
      (char*) args[0], 
      (const char*) args[2],
      NUM_ARG( 4)
    );
  } else if ( 0 == strcmp("strcat", fname)) {
    HANDLE_STRING_CONSTANT( 0);
    HANDLE_STRING_CONSTANT( 2);

    rs->check_strcat(
      (char*) args[0], 
      (const char*) args[2]
    );
  } else if ( 0 == strcmp("strncat", fname)) {
    rs->check_strncat(
      (char*) args[0], 
      (const char*) args[2],
      NUM_ARG( 4)
    );
  } else if ( 0 == strcmp("strchr", fname)) {
    rs->check_strchr(
      (const char*) args[0], 
      (int) NUM_ARG( 2)
    );
  } else if ( 0 == strcmp("strpbrk", fname)) {
    rs->check_strpbrk(
      (const char*) args[0], 
      (const char*) args[2]
    );
  } else if ( 0 == strcmp("strspn", fname)) {
    rs->check_strspn(
      (const char*) args[0], 
      (const char*) args[2]
    );
  } else if ( 0 == strcmp("strstr", fname)) {
    rs->check_strstr(
      (const char*) args[0], 
      (const char*) args[2]
    );
  } else if ( 0 == strcmp("strlen", fname)) {
    rs->check_strlen(
      (const char*) args[0]
    );
  } else {
    cerr << "Function " << fname << " not yet handled." << endl;
    exit(1);
  }

}





/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * fopen, fgetc
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_roseIOFunctionCall(const char* fname,
				    const char* filename, const char* line, const char* lineTransformed,
				   const char* stmtStr, const char* leftHandSideVar, FILE* file,
				   const char* arg1, const char* arg2) {

    //fixme - we need to create a new function call that
    // will have FILE* as parameter
    RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));
  if (strcmp(fname,"fopen")==0) {
    string filen = arg1;
    string mode = arg2;
    int openMode=-1;
    if (strcmp(fname,"r")==0)
      openMode=READ;
    if (strcmp(fname,"w")==0)
      openMode=WRITE;
    rs->registerFileOpen(file, filen, openMode);
  } else if (strcmp(fname,"fgetc")==0) {
    rs->checkFileAccess(file, true);
  } else if (strcmp(fname,"fputc")==0) {
    rs->checkFileAccess(file, false);
  } else if (strcmp(fname,"fclose")==0) {
    rs->registerFileClose(file);
  }

  

}





/*********************************************************
 * This function is called when a function is called
 * The number of parameters is variable but some of the are fixed:
 * count        : number of parameters
 * name         : variable name if it is a variable to be put on the stack
 *              : function name if it is a function that is being called
 * mangled_name : mangled name of the above
 * scope        : Scope indicator (might not be necessary)
 * insertBefore : Indicates whether to push or pop a variable form the stack
 * filename     : file location
 * line         : linenumber
 * stmtStr      : unparsed version of the line to be used for error message
 * ...          : variable amount of additional parameters
 ********************************************************/
void
RuntimeSystem_roseFunctionCall(int count, ...) {
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  const char** args = (const char**)malloc(sizeof(const char*)*count+1);
  int posArgs=0;
  const char* name = NULL;
  const char* filename = NULL;
  const char* line=NULL;
  const char* lineTransf=NULL;
  const char* stmtStr=NULL;
  const char* leftVar=NULL;
  //cerr << "arguments : " <<  count << endl;
  int i=0;
  for ( i=0;i<=count;i++)    {
    const char* val=  va_arg(vl,const char*);
    //    if (val)
    //  printMessage("  %d      val : '%s' ---",i,val);
    //const char *iter2=NULL;
    //int size =0;
    // for ( iter2 = val; *iter2 != '\0'; ++iter2) {
    //  printMessage("%c",*iter2); size++;
    //} printMessage("--- size : %d \n",size);

    if (i==0) name = val;
    else if (i==1) filename =  val;
    else if (i==2) line = val;
    else if (i==3) lineTransf = val;
    else if (i==4) stmtStr = val;
    else if (i==5) leftVar = val;
    else {
      args[posArgs++]=val;
    }
  }
  va_end(vl);


  if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
    // if the string name is one of the above, we handle it specially
    RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  } else if (RuntimeSystem_isFileIOFunctionCall(name)==1) {
    // this will be replaced by a direct call
    //RuntimeSystem_handleIOFunctionCall(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar, NULL);
  } else {
    //printMessage("Unknown Function call to RuntimeSystem!\n");
    exit(1);
  }
}



// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************

void RuntimeSystem_roseEnterScope(const char* name) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
    rs->beginScope(name);

}

void RuntimeSystem_roseExitScope( const char* filename, const char* line, const char* stmtStr) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->endScope();

}




// ***************************************** SCOPE HANDLING *************************************


// ***************************************** VARIABLE DECLARATIONS *************************************
/*********************************************************
 * This function tells the runtime system that a variable is created
 * we store the type of the variable and whether it has been intialized
 ********************************************************/
void RuntimeSystem_roseCreateVariable( const char* name,
				      const char* mangled_name,
				      const char* type,
				      const char* basetype,
              unsigned long int address,
              unsigned int size,
				      int init,
				      const char* fOpen,
				      const char* filename, const char* line,
				      const char* lineTransformed) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint( SourcePosition(filename,atoi(line), atoi(lineTransformed)));
	rs->createVariable(address,name,mangled_name,type,size);

}



/*********************************************************
 * For a given variable name, check if it is present
 * in the pool of variables created and return mangled_name
 ********************************************************/
void
RuntimeSystem_roseInitVariable(const char* name,
			       const char* mangled_name,
			       const char* typeOfVar2,
			       const char* baseType,
			       unsigned long long address,
			       unsigned int size,
			       int ismalloc,
			       const char* filename,
			       const char* line,
			       const char* lineTransformed, const char* stmtStr) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint( SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	//fixme: the size will not work for structs yet!
	rs->checkMemWrite(address,size);

}


/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       unsigned long long address, 
				       unsigned int size,
				       const char* filename, const char* line,
				       const char* lineTransformed,
				       const char* stmtStr) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint( SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	rs->checkMemRead(address, size);

}

// ***************************************** VARIABLE DECLARATIONS *************************************


/*********************************************************
 * Convert an integer to const char*
 ********************************************************/
const char*
RuntimeSystem_roseConvertIntToString(int t) {
  int size = sizeof(int);
  char* text = (char*)malloc(size+1);
  if (text)
    sprintf(text,"%d",t);
  //printMessage("String converted from int : %s ",text);
  return text;
}




void 
RuntimeSystem_roseRegisterTypeCall(int count, ...) {
	  // handle the parameters within this call
	  va_list vl;
	  va_start(vl,count);
	  const char* nameC = va_arg(vl,const char*);
	  const char* typeC = va_arg(vl,const char*);
	  unsigned long long sizeC = va_arg(vl,unsigned long long);
	  cerr << " Register Class : " << nameC << " Type: " << typeC << " size : " << sizeC << endl;
	  int i=0;
	  for ( i=3;i<count;i++)    {
		if ((i%3)==0) {
			cerr << " Var Name : " << va_arg(vl,const char*) << endl;
		}
		if ((i%3)==1) {
			cerr << " Var Type : " << va_arg(vl,const char*) << endl;
		}
		if ((i%3)==2) {
			cerr << " Var offset : " << va_arg(vl,unsigned long long) << endl;
		}
	  }
	  va_end(vl);
}



// vim:sw=2 ts=2 et sta:
