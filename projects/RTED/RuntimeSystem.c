/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * April : Changed this from C++ to C
 * -----------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h> 
#include <assert.h>

#include "RuntimeSystem.h"

/*********************************************************
 * Declare a sole instance of the Runtime System using
 * C Syntax
 ********************************************************/
struct RuntimeSystem* rtsi() {
  static struct RuntimeSystem* rt_pi=0;
  if ( rt_pi == 0)  {
    printf("Creating Instance for RUNTIMESYSTEM ------------------------------------\n");
    //pinstance = new RuntimeSystem(); // create sole instance
    rt_pi = (struct RuntimeSystem*) malloc(sizeof(struct RuntimeSystem)+1);
    if (rt_pi==NULL) {
      printf("INIT FAILED: rt_pi==NULL\n");
      exit(1);
    }
    RuntimeSystem_Const_RuntimeSystem();
  }
  return rt_pi; // address of sole instance
}


/*********************************************************
 * Initialization of all runtime variables and the
 * output file. Result.txt will contain all results for
 * all files
 ********************************************************/
void
RuntimeSystem_Const_RuntimeSystem() { 
  printf(">>>>>>>>>>>>>>>>>>>>>>>>> CONSTRUCTOR \n ................\n\n" );

  //... perform necessary instance initializations 
  // initialze arrays with 10 elements and later increase by 50 if too small
  int initElementSize=0;
  rtsi()->maxRuntimeVariablesEndIndex = rtsi()->maxArraysEndIndex = initElementSize;
  rtsi()->arraysEndIndex=  rtsi()->runtimeVariablesEndIndex=0;
  rtsi()->arrays = (struct arraysType*)malloc(sizeof(struct arraysType)*initElementSize);
  rtsi()->runtimeVariablesOnStack = (struct RuntimeVariablesType*)malloc(sizeof(struct RuntimeVariablesType)*initElementSize);
  rtsi()->myfile = fopen("result.txt","at");
  if (!rtsi()->myfile) {
    printf("Cannot open output file!\n");
    exit(1);
  }
  rtsi()->violation=0; //false
  rtsi()->arrayDebug=0;//false; 
  rtsi()->funccallDebug=1;//true;
}



// ***************************************** HELPER FUNCTIONS *************************************

/*********************************************************
 * resolve the meaning of a boolean value in plain text
 ********************************************************/
char*
RuntimeSystem_resBool(int val) {                                               
  if (val!=0)                                                                      
    return (char*)"true";                                                           
  return (char*)"false";                                                          
}


/*********************************************************
 * return the tail of a string separated by underscore
 ********************************************************/
char*
RuntimeSystem_findLastUnderscore(char* s) {
  char* pos=NULL;
  char* end=NULL;
  char *iter=NULL;
  for ( iter= s; *iter != '\0'; ++iter) {
    if (*iter=='_')
      pos = iter;
    end=iter;
  }
  char* result = NULL;
  if (end!=pos) {
    int size = (end-pos);
    result = (char*) malloc(size+1);
    strcpy(result,pos);
  }
  return result;
}

/*********************************************************
 * This function is called whenever a violation is found
 ********************************************************/
void
RuntimeSystem_callExit(char* filename, char* line, char* reason, char* stmtStr) {
  // rtsi()->violation Found ... dont execute it - exit normally
  printf("rtsi()->Violation found: %s\n  Reason: %s   in file: %s at line %s\n",stmtStr,reason,filename,line);
  fprintf(rtsi()->myfile,"rtsi()->Violation found: %s\n  Reason: %s   in file: %s at line %s\n",stmtStr,reason,filename,line);
  //*myfile << "rtsi()->Violation found: " << stmtStr << endl << "  Reason: " << reason << 
  //  "    in file : " << filename << " at line: " << line << endl;
  exit(0);
}


/*********************************************************
 * Convert an integer to char*
 ********************************************************/
char* 
RuntimeSystem_roseConvertIntToString(int t) {
  int size = sizeof(int);
  char* text = (char*)malloc(size+1);
  if (text)
    sprintf(text,"%d",t);
  printf("String converted from int : %s ",text); 
  return text;
}

/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void 
RuntimeSystem_roseRtedClose() {
  if (rtsi()->myfile)
    //    myfile->close();
    fclose(rtsi()->myfile);
  if (rtsi()->violation==0)  {
    printf("RtedClose:: No rtsi()->violation found!! \n");
    exit(1);
  } else
    printf("RtedClose:: rtsi()->Violation found. Good! \n" );
}

// ***************************************** HELPER FUNCTIONS *************************************








// ***************************************** ARRAY FUNCTIONS *************************************
/*********************************************************
 * For a given variable name, find the entry in arrays[]
 ********************************************************/
int
RuntimeSystem_findArrayName(char* mangled_name) {
  // search by mangled name
  int i=0;
  for (i=0;i<rtsi()->arraysEndIndex;i++) {
    char* name =rtsi()->arrays[i].name;
    //printf(".............findArrayName comparing : %s and %s\n",mangled_name,name);
    if (strcmp(name,mangled_name)==0)
      return i;
  }
  return -1;
}

/*********************************************************
 * Increase the size of the array by multiples of 50
 * array stored all array variables that are used 
 ********************************************************/
void
RuntimeSystem_increaseSizeArray() {                                               
  rtsi()->maxArraysEndIndex+=50;
  struct arraysType* arrays2D_tmp = (struct arraysType*)malloc(sizeof(struct  arraysType)*(rtsi()->maxArraysEndIndex));
  if (arrays2D_tmp) {
    int i=0;
    for ( i=0;i<rtsi()->maxArraysEndIndex;i++) {
      //printf(" %d rtsi()->arrays2D_tmp[i].name = MEM\n",i);
      arrays2D_tmp[i].name=(char*)malloc(sizeof(char*));
      arrays2D_tmp[i].dim =0;
      arrays2D_tmp[i].size1 =0;
      arrays2D_tmp[i].size2 =0;
    }
    for ( i=0;i<rtsi()->arraysEndIndex;i++) {
      //printf(" %d rtsi()->arrays2D_tmp[i].name = %s\n",i,rtsi()->arrays[i].name);
      arrays2D_tmp[i].name=rtsi()->arrays[i].name;
      arrays2D_tmp[i].dim=rtsi()->arrays[i].dim;
      arrays2D_tmp[i].size1 =rtsi()->arrays[i].size1;
      arrays2D_tmp[i].size2 =rtsi()->arrays[i].size2;
    }
    free (rtsi()->arrays);
    rtsi()->arrays=arrays2D_tmp;
  }
  printf( " Increased Array to %d  -- current index %d\n", rtsi()->maxArraysEndIndex, rtsi()->arraysEndIndex );
}

/*********************************************************
 * Increase the size of the stack by multiples of 50
 * stack stored all variables passed via function calls
 ********************************************************/
void
RuntimeSystem_increaseSizeRuntimeVariablesOnStack() {                                        
  rtsi()->maxRuntimeVariablesEndIndex+=50;
  struct RuntimeVariablesType* run_tmp = (struct RuntimeVariablesType*)malloc(sizeof(struct RuntimeVariablesType)*(rtsi()->maxRuntimeVariablesEndIndex));
  if (run_tmp) {
    int i=0;
    for ( i=0;i<rtsi()->maxRuntimeVariablesEndIndex;i++) {
      run_tmp[i].name=(char*)malloc(sizeof(char*));
      run_tmp[i].mangled_name =(char*)malloc(sizeof(char*));
    }
    for ( i=0;i<rtsi()->runtimeVariablesEndIndex;i++) {
      run_tmp[i].name=rtsi()->runtimeVariablesOnStack[i].name;
      run_tmp[i].mangled_name =rtsi()->runtimeVariablesOnStack[i].mangled_name;
    }
    free( rtsi()->runtimeVariablesOnStack);
    rtsi()->runtimeVariablesOnStack=run_tmp;
  }
  printf( " Increased rtsi()->runtimeVariablesOnStack to %d  -- current index %d\n",rtsi()->maxRuntimeVariablesEndIndex ,rtsi()->runtimeVariablesEndIndex  );
}

/*********************************************************
 * For a given variable name, check if it is present
 * on the stack and return the mangled_name
 ********************************************************/
char*
RuntimeSystem_findVariablesOnStack(char* name) {
  char* mang_name = NULL;
  int i=0;
  for ( i=0;i<rtsi()->runtimeVariablesEndIndex;i++) {
    char* n =rtsi()->runtimeVariablesOnStack[i].name;
    if (*name==*n) {
      mang_name=rtsi()->runtimeVariablesOnStack[i].mangled_name;
      break;
    }
  }
  return mang_name;
}



/*********************************************************
 * This function is called when an array is created
 * name      : variable name
 * dimension : 1 or 2
 * stack     : 0 = heap and 1 = stack
 * sizeA     : size of dimension 1
 * sizeB     : size of dimension 2
 * filename  : file location 
 * line      : linenumber
 ********************************************************/
void
RuntimeSystem_roseCreateArray(char* name, int dimension, int stack, long int sizeA, long int sizeB, 
			      char* filename, char* line ){
  if (rtsi()->arrayDebug)
    printf( " >>> Called : roseCreateArray : %s dim %d - [%ld][%ld] file : %s line: %s\n",  
	    RuntimeSystem_findLastUnderscore(name),dimension,sizeA, sizeB, filename, line);

  if (dimension==1) {
    // We create a one dimentional array
    if (rtsi()->arrayDebug)
      printf("rtsi()->arrays1DEndIndex : %d rtsi()->maxArrays1DEndIndex: %d  \n",rtsi()->arraysEndIndex,rtsi()->maxArraysEndIndex);
    if (rtsi()->arraysEndIndex>=rtsi()->maxArraysEndIndex) {
      //increase the size of the array
      RuntimeSystem_increaseSizeArray();
    }
    rtsi()->arrays[rtsi()->arraysEndIndex].name=name;
    rtsi()->arrays[rtsi()->arraysEndIndex].dim=1;
    rtsi()->arrays[rtsi()->arraysEndIndex].size1=sizeA;
    rtsi()->arrays[rtsi()->arraysEndIndex].size2=-1;
    rtsi()->arraysEndIndex++;
      
    if (rtsi()->arrayDebug)
      printf( ".. Creating 1Dim array - size : %ld \n", sizeA);
  }
  else if (dimension==2) {
    // We create a two dimentional array
    int pos = RuntimeSystem_findArrayName(name);
    if (pos!=-1) {
      // this array already exists and may only have one dimension initialized
      long int totalsize = rtsi()->arrays[pos].size1;
      if (rtsi()->arrayDebug)
	printf( "..    Expanding 2nd-run 2Dim array - sizeA : %ld  sizeB: %ld \n", sizeA , sizeB );
      if (sizeA<0 || sizeA>=totalsize) {
	printf( " rtsi()->Violation detected :  Array too small to allocate more memory \n");
	rtsi()->violation=1;
	// this is a weird error, lets stop here for now.
	exit(1);
      } else {
	if (rtsi()->arrayDebug)
	  printf( " >>> CREATING Array : arr [%ld][%ld]  alloc:[%ld]=%ld \n",totalsize,sizeB,sizeA,sizeB);
	rtsi()->arrays[pos].size2=sizeB;
      }
    } else {
      // new array
      if (rtsi()->arraysEndIndex>=rtsi()->maxArraysEndIndex) {
	//increase the size of the array
	RuntimeSystem_increaseSizeArray();
      }
      rtsi()->arrays[rtsi()->arraysEndIndex].name=name;
      rtsi()->arrays[rtsi()->arraysEndIndex].dim=2;
      rtsi()->arrays[rtsi()->arraysEndIndex].size1=sizeA;
      rtsi()->arrays[rtsi()->arraysEndIndex].size2=0;
      rtsi()->arraysEndIndex++;
      
      if (rtsi()->arrayDebug)
	printf( ".. Creating 2Dim array - size : %ld \n", sizeA);
      if (sizeB!=-1) {
	rtsi()->arrays[rtsi()->arraysEndIndex].size2=sizeB;
	if (rtsi()->arrayDebug)
	  printf( "..    Expanding 2Dim array - sizeA : %ld   sizeB: %ld \n", sizeA, sizeB);
      }
    }
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
RuntimeSystem_roseArrayAccess(char* name, int posA, int posB, char* filename, char* line, char* stmtStr){
  if (rtsi()->arrayDebug) 
    printf( "    Called : roseArrayAccess : %s ... ", RuntimeSystem_findLastUnderscore(name));

  // check the stack if the variable is part of a function call
  char* mangl_name=RuntimeSystem_findVariablesOnStack(name);  
  if (mangl_name)
    name=mangl_name;

  int pos = RuntimeSystem_findArrayName(name);
  if (pos!=-1) {
    if (rtsi()->arrays[pos].dim==1) {
      int size = rtsi()->arrays[pos].size1;
      if (rtsi()->arrayDebug)
	printf( "       Found 1Dim array :  size: %d , access [%d] \n", size, posA);
      if (posB!=-1) {
	printf( " Seems like this is not a valid 1Dim array : %s  line: %s\n", filename, line);
	exit(1);
      }
      if (posA>=size || posA<0) {
	char* res1 = ((char*)"Invalid Operation,  array size=");
	char* res2 = ((char*)"  accessing=");
	int sizeInt = 2*sizeof(int);
	char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
	sprintf(res,"%s%d%s%d",res1,size,res2,posA);
	rtsi()->violation=1;
	RuntimeSystem_callExit(filename, line, res, stmtStr);
      }
    }
    else if (rtsi()->arrays[pos].dim==2) {
      int sizeA = rtsi()->arrays[pos].size1;
      int sizeB = rtsi()->arrays[pos].size2;
      if (rtsi()->arrayDebug) 
	printf( "  Found 2Dim array :  size: [%d][%d]  pos: [%d][%d] \n",
		sizeA, sizeB,  posA , posB);
      // allow arr[posA][posB] && arr[posA]  both 2Dim!
      if ((posA>=sizeA || posA<0) || posB>=sizeB || posB<0) {
	char* res1 = ((char*)"Invalid Operation,  array size: [");
	char* res2 = ((char*)"][");
	char* res3 = ((char*)"]   accessing: [");
	char* res4 = ((char*)"][");
	char* res5 = ((char*)"]");
	int sizeInt = 4*sizeof(int);
	char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
	sprintf(res,"%s%d%s%d%s%d%s%d%s",res1,sizeA,res2,sizeB,res3,posA,res4,posB,res5);
	rtsi()->violation=1;
	RuntimeSystem_callExit(filename, line, res,stmtStr);
      }
    }
  } else {
    printf("\n");
    printf( " >>> No such array was created. Can't access it. %s  line : %s \n" , filename, line);
    exit(1);
  }

}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************
/*********************************************************
 * Check if a function call is interesting, i.e. contains a 
 * call to a function that we need to check the parameters of
 ********************************************************/
int 
RuntimeSystem_isInterestingFunctionCall(char* name) {
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
 * Return the number of parameters for a specific function call
 ********************************************************/
int 
RuntimeSystem_getParamtersForFuncCall(char* name) {
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
 * Return the number of parameters for a specific function call
 ********************************************************/
int 
RuntimeSystem_isSizeOfVariableKnown(char* mangled_name) {
  int size=-1;
  int pos = RuntimeSystem_findArrayName(mangled_name);
  if (pos!=-1) {
    if (rtsi()->arrays[pos].dim==1)
      size = rtsi()->arrays[pos].size1;
    else
      // the two dim case is not handled
      assert(0==1);
  }
  return size;
}

/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * memcpy, memmove, strcpy, strncpy, strcat, strncat
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable), 
 *           :          number of char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location 
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void 
RuntimeSystem_handleSpecialFunctionCalls(char* fname,char** args, int argsSize, char* filename, char* line, char* stmtStr) {
  assert(argsSize>=1);
  // parameter 1
  assert(args[0]);
  assert(args[1]);
  int parameters=RuntimeSystem_getParamtersForFuncCall(fname);
  if (parameters==2)
    assert(argsSize>=4);
  if (parameters==3)
    assert(argsSize>=5);

  char* param1StringVal = args[0];
  int param1ActualLength =-1;
  int sizeKnown = RuntimeSystem_isSizeOfVariableKnown(args[1]);
  if (rtsi()->funccallDebug)
    printf("Handling param1 - Size of second paramter is dynamic ? size = %d\n",sizeKnown);
  int param1AllocLength = -2;
  if (sizeKnown!=-1) { // unknown size meaning that variable not found 
    param1AllocLength = sizeKnown;
    //param1ActualLength = 0;//sizeKnown-1;
  } else
    param1AllocLength = atoi(args[1]);
  assert(param1StringVal);

  if (rtsi()->funccallDebug)
    printf("Handling param1-1 - dynamic ? size = %d   param1AllocLength = %d, param1ActualLength = %d\n",sizeKnown,param1AllocLength,param1ActualLength);

  // determine the actual size of each of the strings
  char* end1 = NULL;
  char *iter=NULL;
  int count=0;
  printf("............ Printing : %d  \n",count);
  for ( iter = param1StringVal; *iter != '\0'; ++iter) {
    printf("............ Printing : %d : '%s' \n",count++,iter);
    end1 = iter;
  }
  if (end1==NULL)
    end1= param1StringVal;
  assert(end1);
  int length1 =  (end1-param1StringVal)+1;
  param1ActualLength = length1;

  if (rtsi()->funccallDebug)
    printf("Handling param1-2 - dynamic ? size = %d   param1AllocLength = %d, param1ActualLength = %d\n",sizeKnown,param1AllocLength,param1ActualLength);
  
  assert(param1ActualLength>-1);
  if (rtsi()->funccallDebug)
    printf("1: Number of parameters : %d -- param1AllocLength : %d %s  -- param1ActualLength %d\n",parameters, param1AllocLength, args[1], param1ActualLength);
  if (param1AllocLength==0) {
    // adjust the size of the allocation to the actual size +1
    param1AllocLength=param1ActualLength+1;
    if (rtsi()->funccallDebug)
      printf("1: Expanding : Number of parameters : %d -- param1AllocLength : %d %s  -- param1ActualLength %d\n",parameters, param1AllocLength, args[1], param1ActualLength);
  }


  // parameter 2
  char* param2StringVal = (char*)"";
  int param2ActualLength =-1;
  int param2AllocLength = -2;
  // parameter 3
  int param3Size = 0;

  int sizeKnown2 =-1;
  if (parameters>=2) {
    param2StringVal = args[2];
    param2ActualLength =-1;
    sizeKnown2= RuntimeSystem_isSizeOfVariableKnown(args[3]);
    if (rtsi()->funccallDebug)
      printf("\nHandling param2 - Size of second paramter is dynamic ? size = %d\n",sizeKnown2);
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

  if (rtsi()->funccallDebug)
    printf("Handling param2-1 - dynamic ? size = %d   param2AllocLength = %d, param2ActualLength = %d\n",sizeKnown2,param2AllocLength,param2ActualLength);

  if (parameters>=2) {

      char* end2 = NULL;
      char *iter2=NULL;
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
    if (rtsi()->funccallDebug)
      printf("2: Number of parameters : %d -- param2AllocLength : %d %s  -- param2ActualLength %d\n",parameters, param2AllocLength, args[3], param2ActualLength);
    if (param2AllocLength==0) {
      param2AllocLength=param2ActualLength+1;
      if (rtsi()->funccallDebug)
	printf("2: Expanding : Number of parameters : %d -- param2AllocLength : %d %s  -- param2ActualLength %d\n",parameters, param2AllocLength, args[3], param2ActualLength);
    }
  }
  if (rtsi()->funccallDebug)
    printf("Handling param2-2 - dynamic ? size = %d   param2AllocLength = %d, param2ActualLength = %d\n",sizeKnown2,param2AllocLength,param2ActualLength);



  printf("\nChecking if memory overlaps ... \n");
  printf("(param2StringVal <= param1StringVal) && (param2StringVal+param2ActualLength >= param1StringVal)\n");
  printf("(             %d <= %d             ) && (                                %d >= %d)\n",
	 param2StringVal, param1StringVal, (param2StringVal+param2ActualLength), param1StringVal);
  printf("(param1StringVal <= param2StringVal) && (param1StringVal+param1ActualLength >= param2StringVal)\n");
  printf("(             %d <= %d             ) && (                                %d >= %d)\n",
	 param1StringVal, param2StringVal, (param1StringVal+param1ActualLength), param2StringVal);
  // check for overlapping memory regions
  if (parameters>=2 && 
      (param2StringVal <= param1StringVal) && (param2StringVal+param2ActualLength>=param1StringVal) ||
      (param1StringVal <= param2StringVal) && (param1StringVal+param1ActualLength>=param2StringVal)) {
	if (rtsi()->funccallDebug)
	  printf( " >>>> Error : Memory regions overlap!   Size1: %d  Size2: %d\n",param1ActualLength , param2ActualLength);
	RuntimeSystem_callExit(filename, line, (char*)"Memory regions overlap", stmtStr);  
      } 

  printf("\nChecking if String NULL terminated ... \n");
  char *iter4=NULL;
  int zero1pos=0;
  for ( iter4 = param1StringVal; *iter4 != '\0'; ++iter4) {
    printf("%c",*iter4); zero1pos++;
  } printf("---1 !!!!!!! Found 0 at pos : %d    param1ActualLength %d \n",zero1pos,param1ActualLength);
  // check if the actual size is larger than the allocated size
  if (param1ActualLength>=param1AllocLength) {
    char* res1 = ((char*)"Param1 : Writing outside allocated memory. String possible not NULL terminated. ActualLength = ");
    char* res2 = ((char*)"  AllocLength = ");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,param1ActualLength,res2,param1AllocLength);
    RuntimeSystem_callExit(filename, line, res, stmtStr);  
  } else if ( zero1pos>param1ActualLength) {
    char* res1 = ((char*)"Param1 : String not NULL terminated. ActualLength = ");
    char* res2 = ((char*)"  AllocLength = ");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,param1ActualLength,res2,param1AllocLength);
    RuntimeSystem_callExit(filename, line, res, stmtStr);  
  }

  if (parameters>=2) {
      char *iter3=NULL;
      int zero2pos =0;
      for ( iter3 = param2StringVal; *iter3 != '\0'; ++iter3) {
	printf("%c",*iter3); zero2pos++;
      } printf("---2 !!!!!!! Found 0 at pos : %d     param2ActualLength %d \n",zero2pos,param2ActualLength);

    // check if the actual size is larger than the allocated size
    if (param2ActualLength>=param2AllocLength) {
      char* res1 = ((char*)"Param2 : Writing outside allocated memory. String possible not NULL terminated. ActualLength = ");
      char* res2 = ((char*)"  AllocLength = ");
      int sizeInt = 2*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
      sprintf(res,"%s%d%s%d",res1,param2ActualLength,res2,param2AllocLength);
      RuntimeSystem_callExit(filename, line, res, stmtStr);  
    } else if ( zero2pos>param2ActualLength) {
      char* res1 = ((char*)"Param2 : String not NULL terminated. ActualLength = ");
      char* res2 = ((char*)"  AllocLength = ");
      int sizeInt = 2*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
      sprintf(res,"%s%d%s%d",res1,param2ActualLength,res2,param2AllocLength);
      RuntimeSystem_callExit(filename, line, res, stmtStr);  
    }
  }

  if ( ( strcmp(fname,"strlen")==0 ||  // 1 param
	 strcmp(fname,"strchr")==0 // 1 param
	 )) {
    // checking one parameter for strlen(char* without \0)
    if (rtsi()->funccallDebug)
      printf("CHECK: Special Function call %s  p1: %s act1: %d alloc1: %d   \n", fname,
	     param1StringVal, param1ActualLength, param1AllocLength);
  }
  else 
    if ( ( strcmp(fname ,"strcat")==0  || // 2 param
	   strcmp(fname ,"strncat")==0   // 3 param
	   )) {
      if (rtsi()->funccallDebug)
	printf("CHECK: Special Function call - %s  p1: %s act1: %d alloc1: %d     p2: %s act2: %d alloc2: %d     p3: %d\n", fname,
	       param1StringVal, param1ActualLength, param1AllocLength,
	       param2StringVal, param2ActualLength, param2AllocLength, param3Size);
      // not handled yet
      if (parameters==2) {
	if ((param1ActualLength+param2AllocLength)>=param1AllocLength) {
	  // concatenation above the size of param1AllocLength
	  RuntimeSystem_callExit(filename, line, "Writing beyond memory allocation for 1st parameter", stmtStr);	  
	}
      } else
      if (parameters==3) {
	if ((param1ActualLength+param3Size)>=param1AllocLength) {
	  // concatenation above the size of param1AllocLength
	  RuntimeSystem_callExit(filename, line, "Writing beyond memory allocation for 1st parameter", stmtStr);	  
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
	if (rtsi()->funccallDebug)
	  printf("CHECK: Special Function call - %s  p1: %s act1: %d alloc1: %d     p2: %s act2: %d alloc2: %d   param3Size: %d\n",fname,
		 param1StringVal, param1ActualLength, param1AllocLength,
		 param2StringVal, param2ActualLength, param2AllocLength, param3Size);
	if (parameters==2) {
	  if ((param2AllocLength>param1AllocLength)) {
	    char* res1 = ((char*)"p2 Invalid Operation,  operand1 size=");
	    char* res2 = ((char*)"  operand2 size=");
	    int sizeInt = 2*sizeof(int);
	    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
	    sprintf(res,"%s%d%s%d",res1,param1ActualLength,res2,param2ActualLength);
	    RuntimeSystem_callExit(filename, line, res, stmtStr);
	  }
	} else if (parameters==3) {
	  if ((param3Size>param1ActualLength || param3Size>param2ActualLength)) {
	    // make sure that if the strings do not overlap, they are both smaller than the amount of chars to copy
	    char* res1 = ((char*)"p3 Invalid Operation,  operand1 size=");
	    char* res2 = ((char*)"  operand2 size=");
	    int sizeInt = 2*sizeof(int);
	    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
	    sprintf(res,"%s%d%s%d",res1,param1ActualLength,res2,param2ActualLength);
	    RuntimeSystem_callExit(filename, line, res, stmtStr);
	  } 
	} else assert(1==0);
      }
      else {
	// not handled yet. Need to check if this operation is leagal
	printf("Unhandled special function: Checking special op : %s\n",fname);
	assert(1==0);
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
  printf("RTED - Function Call\n");
  va_list vl;
  va_start(vl,count);
  char** args = (char**)malloc(sizeof(char*)*count+1);
  int posArgs=0;
  char* name = NULL;
  char* filename = NULL;
  char* line=NULL;
  char* stmtStr=NULL;
  //cerr << "arguments : " <<  count << endl;
  int i=0;
  for ( i=0;i<count;i++)    {
    char* val=  va_arg(vl,char*);
    if (val) // && i<4)
      printf("  %d      val : '%s' ---",i,val);
    char *iter2=NULL;
    int size =0;
    for ( iter2 = val; *iter2 != '\0'; ++iter2) {
      printf("%c",*iter2); size++;
    } printf("--- size : %d \n",size);
    
    if (i==0) name = val;
    else if (i==1) filename =  val;
    else if (i==2) line = val;
    else if (i==3) stmtStr = val;
    else {
      args[posArgs++]=val;
    }
  }
  va_end(vl); 


  if (rtsi()->funccallDebug)
    printf( "roseFunctionCall :: %s \n", name );
  if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
    // if the string name is one of the above, we handle it specially
    RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, stmtStr);
  } 
}


/*********************************************************
 * This function is called when a variable is put or dropped from stack
 * stack variables are used to keep track of what variables are passed
 * to functions. Their mangled_names help to identify the real definitions.
 * name         : variable name if it is a variable to be put on the stack
 * mangled_name : mangled name of the above
 * insertBefore : Indicates whether to push or pop a variable form the stack
 ********************************************************/
void 
RuntimeSystem_roseCallStack(char* name, char* mangl_name, char* beforeStr) {
  // we want to remember the varRefs that are passed via function calls to functions
  // if before ==true
  // add the current varRef (name) on stack
  // else remove from stack
  int before =0; // false
  if (strcmp(beforeStr,"true")==0)
    before=1;

  if (before) {
    if (rtsi()->runtimeVariablesEndIndex>=rtsi()->maxRuntimeVariablesEndIndex) {
      //increase the size of the array
      RuntimeSystem_increaseSizeRuntimeVariablesOnStack();
    }
    rtsi()->runtimeVariablesOnStack[rtsi()->runtimeVariablesEndIndex].name=name;
    rtsi()->runtimeVariablesOnStack[rtsi()->runtimeVariablesEndIndex].mangled_name=mangl_name;
    rtsi()->runtimeVariablesEndIndex++;

  }
  else {
    rtsi()->runtimeVariablesEndIndex--;
    //      rtsi()->runtimeVariablesOnStack[rtsi()->runtimeVariablesEndIndex].name=NULL;
    //rtsi()->runtimeVariablesOnStack[rtsi()->runtimeVariablesEndIndex].mangled_name=NULL;

    //RuntimeVariables* var = rtsi()->runtimeVariablesOnStack.back();
    //rtsi()->runtimeVariablesOnStack.pop_back();
    //delete var;
  }

}
// ***************************************** FUNCTION CALL *************************************



