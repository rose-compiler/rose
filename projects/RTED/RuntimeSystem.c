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
  assert(argsSize>=4);
  char* param1StringVal = args[0];
  char* param1ManglName = args[1];
  char* param2StringVal = args[2];
  char* param2ManglName = args[3];
  assert(param1StringVal);
  assert(param2StringVal);
  //assert(param1ManglName);
  //assert(param2ManglName);

  int param3Size = 0;
  if (argsSize>4)
    param3Size = strtol(args[4],NULL,10);
  if (rtsi()->funccallDebug)
    printf( "  >>Runtimesystem :: normalFunctionCall  -  param1StringVal : \"%s\"   param2StringVal : \"%s\"   param3Size: %d \n",param1StringVal,param2StringVal,param3Size);
  // check if a variable with that name exists!!
  //  char* mangl_name =RuntimeSystem_findVariablesOnStack(param1StringVal);  
  int pos = -1;
  if (strcmp(param1StringVal,"")==0) {
    printf("    param1StringVal==NULL \n");
    pos = RuntimeSystem_findArrayName(param1ManglName);
  }
  int sizeString1 = -1;
  char* end1 = NULL;
  if (pos!=-1) {
    int size = rtsi()->arrays[pos].size1;
    printf("Found variable, pos : %d  size : %d \n",pos,size);
    sizeString1 = size;
    // the pointer to the end of the memory block
    end1 = param1ManglName+sizeString1;
  } else {
    printf("Variable not found!! , pos : %d  size : %d \n",pos,sizeString1);
    char *iter=NULL;
    for ( iter = param1StringVal; *iter != '\0'; ++iter) {
      end1 = iter;
    }
    assert(end1);
    sizeString1 = (end1-param1StringVal)+1;
  }

  pos = -1;
  if (strcmp(param2StringVal,"")==0) {
    printf("    param2StringVal==NULL \n");
    pos = RuntimeSystem_findArrayName(param2ManglName);
  }
  int sizeString2 = -1;
  char* end2 = NULL;
  if (pos!=-1) {
    int sizeA = rtsi()->arrays[pos].size1;
    // if the copy size==0 then we copy the entire string
    if (param3Size==0)
      param3Size = sizeA;
    printf("Found variable 2, pos : %d  size : %d \n",pos,sizeA);
    sizeString2 = sizeA;
    // the pointer to the end of the memory block
    end2 = param2ManglName+sizeString2;
  } else {
    printf("Variable 2 not found!! , pos : %d  size : %d \n",pos,sizeString1);
    char *iter2=NULL;
    for ( iter2 = param2StringVal; *iter2 != '\0'; ++iter2) {
      end2 = iter2;
    }
    assert(end2);
    sizeString2 = (end2-param2StringVal)+1;
    if (param3Size==0)
      param3Size = sizeString2;
  }
  assert(sizeString1>-1);
  assert(sizeString2>-1);
  assert(param3Size>0);

  // check if string1 and string2 overlap. Dont allow memcopy on such
  if (rtsi()->funccallDebug)
    printf( " >>> FunctionCall : Checking param1StringVal=<%s> (%d)  and param2StringVal=<%s> (%d)  sizeOp: %d \n", 
	    param1StringVal, sizeString1, param2StringVal , sizeString2 , param3Size );
  if (end1 >= param2StringVal && param1StringVal<=end2) {
    // overlapping memory regions
    if (rtsi()->funccallDebug)
      printf( " >>>> Error : Memory regions overlap!   Size1: %d  Size2: %d\n",sizeString1 , sizeString2);
    RuntimeSystem_callExit(filename, line, (char*)"Memory regions overlap", stmtStr);  
  } else if (param3Size>0 && (param3Size>sizeString1 || param3Size>sizeString2)) {
    // make sure that if the strings do not overlap, they are both smaller than the amount of chars to copy
    char* res1 = ((char*)"Invalid Operation,  operand1 size=");
    char* res2 = ((char*)"  operand2 size=");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,sizeString1,res2,sizeString2);
    RuntimeSystem_callExit(filename, line, res, stmtStr);
  } 
  printf("No problem found!\n");

  if (
      strcmp(fname ,"strcat")==0 ||
      strcmp(fname ,"strncat")==0) {

    // not handled yet. Need to check if this operation is leagal
    printf("Checking special op : %s\n",fname);
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
  va_list vl;
  va_start(vl,count);
  char** args = (char**)malloc(sizeof(char*)*count+1);
  int posArgs=0;
  char* name = NULL;
  char* mangl_name = NULL;
  char* beforeStr = NULL;
  char* scope_name = NULL;
  char* filename = NULL;
  char* line=NULL;
  // if before then we put it on the stack
  // after we remove the variable from the stack
  int before=0; //false
  char* stmtStr=NULL;
  //cerr << "arguments : " <<  count << endl;
  int i=0;
  for ( i=0;i<count;i++)    {
    char* val=  va_arg(vl,char*);
    if (i==0) name = val;
    else if (i==1) mangl_name =  val;
    else if (i==2) scope_name =  val;
    else if (i==3) {
      beforeStr = val;
      if (strcmp(beforeStr,"true")==0)
	before=1;
    }
    else if (i==4) filename =  val;
    else if (i==5) line = val;
    else if (i==6) stmtStr = val;
    else {
      args[posArgs++]=val;
    }
  }
  va_end(vl); 


  if (rtsi()->funccallDebug)
    printf( "roseFunctionCall :: %s %s %s\n", name , mangl_name, beforeStr);
  if (before && ( strcmp(name,"memcpy")==0 || 
		  strcmp(name ,"memmove")==0 || 
		  strcmp(name ,"strcpy")==0 ||
		  strcmp(name ,"strncpy")==0 ||
		  strcmp(name ,"strcat")==0 ||
		  strcmp(name ,"strncat")==0
		  )) {
    // if the string name is one of the above, we handle it specially
    RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, stmtStr);
  } else {
    // we want to remember the varRefs that are passed via function calls to functions
    // if before ==true
    // add the current varRef (name) on stack
    // else remove from stack
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
}


// ***************************************** FUNCTION CALL *************************************



