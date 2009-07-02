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

#include "RuntimeSystem.h"
#include "CppRuntimeSystem.h"
#include "rted_qt/rted_qt.h"



/*********************************************************
 * Constructor
 ********************************************************/
void
RuntimeSystem_Const_RuntimeSystem() {
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs->setOutputFile("result.txt");
}

/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void
RuntimeSystem_roseRtedClose() {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->doProgramExitChecks();
}







// ***************************************** ARRAY FUNCTIONS *************************************





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
RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name, int dimension,  long int sizeA, long int sizeB,
			      int ismalloc, const char* filename, const char* line, const char* lineTransformed){
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint(SourcePosition(filename,line,lineTransformed));
	// fixme: address
	if (dimension==1)
		rs->createMemory(address, sizeA);
	else
		rs->createMemory(address, sizeA*sizeB);

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
			      const char* line, const char* lineTransformed, const char* stmtStr){
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint(SourcePosition(filename,line,lineTransformed));
	// fixme : need address and size
	if (read)
		rs->checkMemRead(address,sizeOf);
}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************
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
  assert(argsSize>=1);
  // parameter 1
  assert(args[0]);
  assert(args[1]);
  int parameters=RuntimeSystem_getParamtersForFuncCall(fname);
  if (parameters==2)
    assert(argsSize>=4);
  if (parameters==3)
    assert(argsSize>=5);

  const char* param1StringVal = args[0];
  int param1ActualLength =-1;
  int sizeKnown = RuntimeSystem_isSizeOfVariableKnown(args[1]);
  if (rs->funccallDebug)
    printMessage("Handling param1 - Size of second paramter is dynamic ? size = %d\n",sizeKnown);
  int param1AllocLength = -2;
  if (sizeKnown!=-1) { // unknown size meaning that variable not found
    param1AllocLength = sizeKnown;
    //param1ActualLength = 0;//sizeKnown-1;
  } else
    param1AllocLength = atoi(args[1]);
  assert(param1StringVal);

  if (rs->funccallDebug)
    printMessage("Handling param1-1 - dynamic ? size = %d   param1AllocLength = %d, param1ActualLength = %d\n",sizeKnown,param1AllocLength,param1ActualLength);

  // determine the actual size of each of the strings
  // we need to do this only for certain functions!
  // This is important because otherwise e.g. strcpy
  // will show null terminated string also this is not relevant
  // for str1=malloc(5);  strcpy(str1,"la")
  int modifyingCall = RuntimeSystem_isModifyingOp(fname);
  if (modifyingCall==0) {
    const char* end1 = NULL;
    const char *iter=NULL;
    int count=0;
    printMessage("............ Printing : %d  \n",count);
    for ( iter = param1StringVal; *iter != '\0'; ++iter) {
      printMessage("............ Printing : %d : '%s' \n",count++,iter);
      end1 = iter;
    }
    if (end1==NULL)
      end1= param1StringVal;
    assert(end1);
    int length1 =  (end1-param1StringVal)+1;
    param1ActualLength = length1;
  } else {
    // if the operation is modifying, e.g. memcpy, we want the actual size to be 0
    param1ActualLength=0;
  }

  if (rs->funccallDebug)
    printMessage("Handling param1-2 - dynamic ? size = %d   param1AllocLength = %d, param1ActualLength = %d     isModifyingCall : %d \n",sizeKnown,param1AllocLength,param1ActualLength, modifyingCall);

  assert(param1ActualLength>-1);
  if (rs->funccallDebug)
    printMessage("1: Number of parameters : %d -- param1AllocLength : %d %s  -- param1ActualLength %d\n",parameters, param1AllocLength, args[1], param1ActualLength);
  if (param1AllocLength==0) {
    // adjust the size of the allocation to the actual size +1
    param1AllocLength=param1ActualLength+1;
    if (rs->funccallDebug)
      printMessage("1: Expanding : Number of parameters : %d -- param1AllocLength : %d %s  -- param1ActualLength %d\n",parameters, param1AllocLength, args[1], param1ActualLength);
  }


  // parameter 2
  const char* param2StringVal = (const char*)"";
  int param2ActualLength =-1;
  int param2AllocLength = -2;
  // parameter 3
  int param3Size = 0;

  int sizeKnown2 =-1;
  if (parameters>=2) {
    param2StringVal = args[2];
    param2ActualLength =-1;
    sizeKnown2= RuntimeSystem_isSizeOfVariableKnown(args[3]);
    if (rs->funccallDebug)
      printMessage("\nHandling param2 - Size of second paramter is dynamic ? size = %d\n",sizeKnown2);
    if (sizeKnown2!=-1) {
      param2AllocLength = sizeKnown2;
      //      param2ActualLength = 0;//sizeKnown2-1;
    } else
      param2AllocLength = atoi(args[3]);
    assert(param2StringVal);
  }
  if (parameters==3) {
    // determine the 3rd parameter if present, e.g. strcpy(p1,p2,size);
    if (argsSize>4)
      param3Size = strtol(args[4],NULL,10);
  }

  if (rs->funccallDebug)
    printMessage("Handling param2-1 - dynamic ? size = %d   param2AllocLength = %d, param2ActualLength = %d\n",sizeKnown2,param2AllocLength,param2ActualLength);

  if (parameters>=2) {

    const char* end2 = NULL;
    const char *iter2=NULL;
    for ( iter2 = param2StringVal; *iter2 != '\0'; ++iter2) {
      end2 = iter2;
    }
    if (end2==NULL)
      end2= param2StringVal;
    assert(end2);
    int length2 =  (end2-param2StringVal)+1;
    param2ActualLength = length2;


    if (param3Size==0)
      param3Size = param2ActualLength;
    assert(param2ActualLength>-1);
    assert(param3Size>0);
    if (rs->funccallDebug)
      printMessage("2: Number of parameters : %d -- param2AllocLength : %d %s  -- param2ActualLength %d\n",parameters, param2AllocLength, args[3], param2ActualLength);
    if (param2AllocLength==0) {
      param2AllocLength=param2ActualLength+1;
      if (rs->funccallDebug)
	printMessage("2: Expanding : Number of parameters : %d -- param2AllocLength : %d %s  -- param2ActualLength %d\n",parameters, param2AllocLength, args[3], param2ActualLength);
    }
  }
  if (rs->funccallDebug)
    printMessage("Handling param2-2 - dynamic ? size = %d   param2AllocLength = %d, param2ActualLength = %d\n",sizeKnown2,param2AllocLength,param2ActualLength);



  printMessage("\nChecking if memory overlaps ... \n");
  printMessage("(param2StringVal <= param1StringVal) && (param2StringVal+param2AllocLength >= param1StringVal)\n");
  printMessage("(             %d <= %d             ) && (                                %d >= %d)\n",
	 param2StringVal, param1StringVal, (param2StringVal+param2ActualLength), param1StringVal);
  printMessage("(param1StringVal <= param2StringVal) && (param1StringVal+param1AllocLength >= param2StringVal)\n");
  printMessage("(             %d <= %d             ) && (                                %d >= %d)\n",
	 param1StringVal, param2StringVal, (param1StringVal+param1ActualLength), param2StringVal);
  // check for overlapping memory regions
  if (parameters>=2 &&
      (param2StringVal <= param1StringVal) && (param2StringVal+param2AllocLength>=param1StringVal) ||
      (param1StringVal <= param2StringVal) && (param1StringVal+param1AllocLength>=param2StringVal)) {
    if (rs->funccallDebug)
      printMessage( " >>>> Error : Memory regions overlap!   Size1: %d  Size2: %d\n",param1ActualLength , param2ActualLength);
    RuntimeSystem_callExit(filename, line, (const char*)"Memory regions overlap", stmtStr);
  }

  printMessage("\nChecking if String NULL terminated ... \n");
  const char *iter4=NULL;
  int zero1pos=0;
  if (modifyingCall==0) {
    // do not check for the end const character of first operator if this is a modifying call, e.g. strcpy
    for ( iter4 = param1StringVal; *iter4 != '\0'; ++iter4) {
      printMessage("%c",*iter4); zero1pos++;
    } printMessage("---1 !!!!!!! Found 0 at pos : %d    param1ActualLength %d \n",zero1pos,param1ActualLength);
  }
  // check if the actual size is larger than the allocated size
  if ( zero1pos>param1ActualLength) {
    const char* res1 = ((const char*)"Param1 : String not NULL terminated. zero1pos: ");
    const char* res2 = ((const char*)"  > ActualLength:");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,zero1pos,res2,param1ActualLength);
    RuntimeSystem_callExit(filename, line, res, stmtStr);
  } else if (param1ActualLength>=param1AllocLength) {
    const char* res1 = ((const char*)"Param1 : Writing/Reading outside allocated memory.  ActualLength = ");
    const char* res2 = ((const char*)" >= AllocLength = ");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,param1ActualLength,res2,param1AllocLength);
    RuntimeSystem_callExit(filename, line, res, stmtStr);
  }

  if (parameters>=2) {
    const char *iter3=NULL;
    int zero2pos =0;
    for ( iter3 = param2StringVal; *iter3 != '\0'; ++iter3) {
      printMessage("%c",*iter3); zero2pos++;
    } printMessage("---2 !!!!!!! Found 0 at pos : %d     param2ActualLength %d \n",zero2pos,param2ActualLength);

    // check if the actual size is larger than the allocated size
    if ( zero2pos>param2ActualLength) {
      const char* res1 = ((const char*)"Param2 : String not NULL terminated. zero2pos: ");
      const char* res2 = ((const char*)"  > ActualLength:");
      int sizeInt = 2*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
      sprintf(res,"%s%d%s%d",res1,zero2pos,res2,param2ActualLength);
      RuntimeSystem_callExit(filename, line, res, stmtStr);
    } else if (param2ActualLength>=param2AllocLength) {
      const char* res1 = ((const char*)"Param2 : Writing/Reading outside allocated memory. ActualLength:");
      const char* res2 = ((const char*)" >= AllocLength:");
      int sizeInt = 2*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
      sprintf(res,"%s%d%s%d",res1,param2ActualLength,res2,param2AllocLength);
      RuntimeSystem_callExit(filename, line, res, stmtStr);
    }
  }

  if ( ( strcmp(fname,"strlen")==0 ||  // 1 param
	 strcmp(fname,"strchr")==0 // 1 param
	 )) {
    // checking one parameter for strlen(const char* without \0)
    if (rs->funccallDebug)
      printMessage("CHECK: Special Function call %s  p1: %s act1: %d alloc1: %d   \n", fname,
	     param1StringVal, param1ActualLength, param1AllocLength);
  }
  else
    if ( ( strcmp(fname ,"strcat")==0  || // 2 param
	   strcmp(fname ,"strncat")==0   // 3 param
	   )) {
      if (rs->funccallDebug)
	printMessage("CHECK: Special Function call - %s  p1: %s act1: %d alloc1: %d     p2: %s act2: %d alloc2: %d     p3: %d\n", fname,
	       param1StringVal, param1ActualLength, param1AllocLength,
	       param2StringVal, param2ActualLength, param2AllocLength, param3Size);
      // not handled yet
      if (parameters==2) {
	if ((param1ActualLength+param2AllocLength)>=param1AllocLength) {
	  // concatenation above the size of param1AllocLength
	  RuntimeSystem_callExit(filename, line, (const char*)"Writing beyond memory allocation for 1st parameter", stmtStr);
	}
      } else
	if (parameters==3) {
	  if ((param1ActualLength+param3Size)>=param1AllocLength) {
	    // concatenation above the size of param1AllocLength
	    RuntimeSystem_callExit(filename, line, (const char*)"Writing beyond memory allocation for 1st parameter", stmtStr);
	  }
	}

    }
    else
      if ( ( strcmp(fname,"memcpy")==0   || // 3 param
	     strcmp(fname ,"memmove")==0 || // 3 param
	     strcmp(fname ,"strcpy")==0  || // 2 param
	     strcmp(fname ,"strncpy")==0 || // 3 param
	     strcmp(fname ,"strpbrk")==0 || // 2 param
	     strcmp(fname ,"strspn")==0  || // 2 param
	     strcmp(fname ,"strstr")==0  // 2 param
	     )) {
	if (rs->funccallDebug)
	  printMessage("CHECK: Special Function call - %s  p1: %s act1: %d alloc1: %d     p2: %s act2: %d alloc2: %d   param3Size: %d\n",fname,
		 param1StringVal, param1ActualLength, param1AllocLength,
		 param2StringVal, param2ActualLength, param2AllocLength, param3Size);
	if (parameters==2) {
	  if ((param2AllocLength>param1AllocLength)) {
	    const char* res1 = ((const char*)"p2 Invalid Operation,  operand2:");
	    const char* res2 = ((const char*)"  >  operand1:");
	    int sizeInt = 2*sizeof(int);
	    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
	    sprintf(res,"%s%d%s%d",res1,param2AllocLength,res2,param1AllocLength);
	    RuntimeSystem_callExit(filename, line, res, stmtStr);
	  }
	} else if (parameters==3) {
	  if ((param3Size>param1AllocLength || param3Size>param2AllocLength)) {
	    // make sure that if the strings do not overlap, they are both smaller than the amount of const chars to copy
	    const char* res1 = ((const char*)"p3 Invalid Operation,  operand3:");
	    const char* res2 = ((const char*)"  >  operand1:");
	    const char* res3 = ((const char*)"  or   operand3:");
	    const char* res4 = ((const char*)"  >  operand2:");
	    int sizeInt = 4*sizeof(int);
	     char *res = ( char*)malloc(strlen(res1) + strlen(res2) +strlen(res3) + strlen(res4)+sizeInt+ 1);
	    sprintf(res,"%s%d%s%d%s%d%s%d",res1,param3Size,res2,param1AllocLength,res3,
		    param3Size,res4,param2AllocLength);
	    RuntimeSystem_callExit(filename, line, res, stmtStr);
	  }
	} else assert(1==0);
      }
      else {
	// not handled yet. Need to check if this operation is leagal
	printMessage("Unhandled special function: Checking special op : %s\n",fname);
	assert(1==0);
      }

  Rted_debugDialog(filename, atoi(line), atoi(lineTransformed));

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
RuntimeSystem_handleIOFunctionCall(const char* fname,const char** args,
				   int argsSize, const char* filename, const char* line, const char* lineTransformed,
				   const char* stmtStr, const char* leftHandSideVar) {
  assert(argsSize>=1);
  // parameter 1
  int parameters=RuntimeSystem_getParamtersForFuncCall(fname);
  if  (strcmp(fname,"fopen")==0) {
    // need 4 parameters, var, size, var, size
    assert(argsSize>=4);
    // we need to mark the variable with fopen that it is open for read/write
    const char* file = args[0];
    const char* fileLength = args[1];
    const char* readwrite = args[2];
    const char* readwriteLength = args[3];
    if(!leftHandSideVar) assert(0==1);
    struct RuntimeVariablesType* rvar = RuntimeSystem_findVariables(leftHandSideVar);
    if (leftHandSideVar) {
      printMessage("Making sure that the var %s is set to : %s \n",leftHandSideVar,readwrite);
      rvar->fileOpen=readwrite;
      // make sure that the file exists
      FILE* fp = fopen(file,"r");
      if (fp==0) {
	// file does not exist
	RuntimeSystem_callExit(filename, line, (const char*)"No such file. Can not open this file.", stmtStr);
      }
    } else {
      printMessage("File opend: This must be an error, The variable %s in fopen should be available.\n",leftHandSideVar);
      exit(1);
    }
  }
  else  if  (strcmp(fname,"fclose")==0) {
    // need 4 parameters, var, size, var, size
    assert(argsSize==1);
    // we need to mark the variable with fopen that it is open for read/write
    const char* var = args[0];
    assert(var);
    struct RuntimeVariablesType* rvar = RuntimeSystem_findVariables(var);
    if (rvar==NULL) {
      const char* stackvar = RuntimeSystem_findVariablesOnStack(var);
      if (stackvar) {
	printMessage("Found the variable on the stack : %s \n",stackvar);
	rvar = RuntimeSystem_findVariables(stackvar);
      }
    }
    if (rvar) {
      if (strcmp(rvar->fileOpen,"no")==0) {
	// file closed although it never was opend
	RuntimeSystem_callExit(filename, line, (const char*)"Closing a file that never was opened.", stmtStr);
      } else {
	printMessage("File closed for var: %s \n",rvar->name);
	rvar->fileOpen=(const char*)"no";
      }
    } else {
      printMessage("File closed : This must be an error, The variable %s in fopen should be available.\n",var);
      exit(1);
    }
  }
  else if  (strcmp(fname,"fgetc")==0 ) {
    // only one parameter
    assert(argsSize==1);
    const char* var = args[0];
    // check if the variable used : fgetc(var) is a pointer to a file
    // that has been opened already, i.e. is file open?
    struct RuntimeVariablesType* rvar = RuntimeSystem_findVariables(var);
    if (rvar) {
      printMessage("RuntimeSystem : RuntimeSystem_handleIOFunctionCall : found Variable %s (%s)  fileOpen? %s \n",
	     var, rvar->name, rvar->fileOpen);
      if (strcmp(rvar->fileOpen,"no")==0) {
	// the file is not open, we cant read/write
	RuntimeSystem_callExit(filename, line, (const char*)"File not open. Can't read/write to file.", stmtStr);
      }
      if (strstr( rvar->fileOpen, "r" )==0) {
	// reading a file that is not marked for reading
	RuntimeSystem_callExit(filename, line, (const char*)"Can not read from File. File not opened for reading.", stmtStr);
      }
    }
  }
  else if  (strcmp(fname,"fputc")==0 ) {
    // only one parameter
    assert(argsSize==2);
    const char* filestream = args[0];
    const char* var = args[1];
    // check if the variable used : fgetc(var) is a pointer to a file
    // that has been opened already, i.e. is file open?
    struct RuntimeVariablesType* rvar = RuntimeSystem_findVariables(var);
    if (var) {
      printMessage("RuntimeSystem : RuntimeSystem_handleIOFunctionCall : found Variable %s (%s)  fileOpen? %s \n",
	     var, rvar->name, rvar->fileOpen);
      if (strcmp(rvar->fileOpen,"no")==0) {
	// the file is not open, we cant read/write
	RuntimeSystem_callExit(filename, line, (const char*)"File not open. Can't read/write to file.", stmtStr);
      }
      if (strstr( rvar->fileOpen, "w" )==0) {
	// reading a file that is not marked for reading
	RuntimeSystem_callExit(filename, line, (const char*)"Can not write to File. File not opened for writing.", stmtStr);
      }

    }
  }

  Rted_debugDialog(filename, atoi(line),atoi(lineTransformed));

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
  printMessage("RTED - Function Call\n");
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
  for ( i=0;i<count;i++)    {
    const char* val=  va_arg(vl,const char*);
    if (val)
      printMessage("  %d      val : '%s' ---",i,val);
    const char *iter2=NULL;
    int size =0;
    for ( iter2 = val; *iter2 != '\0'; ++iter2) {
      printMessage("%c",*iter2); size++;
    } printMessage("--- size : %d \n",size);

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


  if (rs->funccallDebug)
    printMessage( "roseFunctionCall :: %s \n", name );
  if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
    // if the string name is one of the above, we handle it specially
    RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  } else if (RuntimeSystem_isFileIOFunctionCall(name)==1) {
    RuntimeSystem_handleIOFunctionCall(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  } else {
    printMessage("Unknown Function call to RuntimeSystem!\n");
    exit(1);
  }
}



// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************
// TODO 1 djh: check for missing frees

void RuntimeSystem_roseEnterScope() {
	RuntimeSystem * rs = RuntimeSystem::instance();
    rs->beginScope(name);
}

void RuntimeSystem_roseExitScope( const char* filename, const char* line, const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
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
				      int init,
				      const char* fOpen,
				      const char* filename, const char* line,
				      const char* lineTransformed) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,line,lineTransformed));
	rs->createVariable(address,name,mangled_name,type);
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
			       unsigned long long value,
			       int ismalloc,
			       const char* filename,
			       const char* line,
			       const char* lineTransformed, const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,line,lineTransformed));
	rs->checkMemWrite(address,?);
}


/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       const char* filename, const char* line,
				       const char* lineTransformed,
				       const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,line,lineTransformed));
	rs->checkMemRead(address,?);
}

// ***************************************** VARIABLE DECLARATIONS *************************************

// vim:sw=2 ts=2 et sta:
