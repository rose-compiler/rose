/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <cstdio>
#include <stdarg.h> 
#include <assert.h>

#include "RuntimeSystem.h"


//struct RuntimeSystem* rt_pi;
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


void
RuntimeSystem_Const_RuntimeSystem() { 
  printf(">>>>>>>>>>>>>>>>>>>>>>>>> CONSTRUCTOR \n ................\n\n" );

  //... perform necessary instance initializations 
  // initialze arrays with 10 elements and later increase by 50 if too small
  int initElementSize=0;
  rtsi()->maxRuntimeVariablesEndIndex = rtsi()->maxArrays1DEndIndex = rtsi()->maxArrays2DEndIndex = initElementSize;
  rtsi()->arrays1DEndIndex=  rtsi()->arrays2DEndIndex=  rtsi()->runtimeVariablesEndIndex=0;
  rtsi()->arrays1D = (struct arrays1DType*)malloc(sizeof(struct arrays1DType)*initElementSize);
  rtsi()->arrays2D = (struct arrays2DType*)malloc(sizeof(struct arrays2DType)*initElementSize);
  rtsi()->runtimeVariablesOnStack = (struct RuntimeVariablesType*)malloc(sizeof(struct RuntimeVariablesType)*initElementSize);
  rtsi()->myfile = fopen("result.txt","at");
  //myfile = new std::ofstream("result.txt",std::ios::app);
  if (!rtsi()->myfile) {
    printf("Cannot open output file!\n");
    exit(1);
  }
  rtsi()->violation=0; //false
  rtsi()->arrayDebug=0;//false; 
  rtsi()->funccallDebug=1;//true;
}



// ***************************************** HELPER FUNCTIONS *************************************

int
RuntimeSystem_findArrayName1D(char* n) {
  int i=0;
  for (i=0;i<rtsi()->arrays1DEndIndex;i++) {
    char* name =rtsi()->arrays1D[i].name;
    if (*name==*n)
      return i;
  }
  return -1;
}

int
RuntimeSystem_findArrayName2D(char* n) {
  int i=0;
  for (i=0;i<rtsi()->arrays2DEndIndex;i++) {
    char* name =rtsi()->arrays2D[i].name;
    if (*name==*n)
      return i;
  }
  return -1;
}

void
RuntimeSystem_increaseSizeArray1() {   
  rtsi()->maxArrays1DEndIndex+=50;
  struct arrays1DType* arrays1D_tmp = (struct arrays1DType*)malloc(sizeof(struct arrays1DType)*(rtsi()->maxArrays1DEndIndex));
  if (arrays1D_tmp) {
    int i=0;
    for (i=0;i<rtsi()->maxArrays1DEndIndex;i++) {
      //printf(" %d rtsi()->arrays1D_tmp[i].name = MEM\n",i);
	arrays1D_tmp[i].name=(char*)malloc(sizeof(char*));
	arrays1D_tmp[i].size1 =0;
    }
    for ( i=0;i<rtsi()->arrays1DEndIndex;i++) {
      //printf(" %d rtsi()->arrays1D_tmp[i].name = %s\n",i,rtsi()->arrays1D[i].name);
	arrays1D_tmp[i].name=rtsi()->arrays1D[i].name;
	arrays1D_tmp[i].size1 =rtsi()->arrays1D[i].size1;
    }
    free (rtsi()->arrays1D);
    rtsi()->arrays1D=arrays1D_tmp;
  }
  printf( " Increased Array1 to %d  -- current index %d\n", rtsi()->maxArrays1DEndIndex, rtsi()->arrays1DEndIndex );

}

void
RuntimeSystem_increaseSizeArray2() {                                               
  rtsi()->maxArrays2DEndIndex+=50;
  struct arrays2DType* arrays2D_tmp = (struct arrays2DType*)malloc(sizeof(struct  arrays2DType)*(rtsi()->maxArrays2DEndIndex));
  if (arrays2D_tmp) {
    int i=0;
    for ( i=0;i<rtsi()->maxArrays2DEndIndex;i++) {
      //printf(" %d rtsi()->arrays2D_tmp[i].name = MEM\n",i);
	arrays2D_tmp[i].name=(char*)malloc(sizeof(char*));
	arrays2D_tmp[i].size1 =0;
	arrays2D_tmp[i].size2 =0;
    }
    for ( i=0;i<rtsi()->arrays2DEndIndex;i++) {
      //printf(" %d rtsi()->arrays2D_tmp[i].name = %s\n",i,rtsi()->arrays2D[i].name);
	arrays2D_tmp[i].name=rtsi()->arrays2D[i].name;
	arrays2D_tmp[i].size1 =rtsi()->arrays2D[i].size1;
	arrays2D_tmp[i].size2 =rtsi()->arrays2D[i].size2;
    }
    free (rtsi()->arrays2D);
    rtsi()->arrays2D=arrays2D_tmp;
  }
  printf( " Increased Array2 to %d  -- current index %d\n", rtsi()->maxArrays2DEndIndex, rtsi()->arrays2DEndIndex );
}

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


char*
RuntimeSystem_resBool(int val) {                                               
  if (val!=0)                                                                      
    return (char*)"true";                                                           
  return (char*)"false";                                                          
}


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
#if 0
  string ss = roseConvertToString(s);
  string name = "";
  int pos = ss.rfind("_");
  if (pos!=(int)std::string::npos) {
    name = ss.substr(pos,ss.length());
  }
  return (char*) name.c_str();
#endif
}

void
RuntimeSystem_callExit(char* filename, char* line, char* reason, char* stmtStr) {
  // rtsi()->violation Found ... dont execute it - exit normally
  printf("rtsi()->Violation found: %s\n  Reason: %s   in file: %s at line %s\n",stmtStr,reason,filename,line);
  fprintf(rtsi()->myfile,"rtsi()->Violation found: %s\n  Reason: %s   in file: %s at line %s\n",stmtStr,reason,filename,line);
  //*myfile << "rtsi()->Violation found: " << stmtStr << endl << "  Reason: " << reason << 
  //  "    in file : " << filename << " at line: " << line << endl;
  exit(0);
}

#if 0
template<typename T> std::string 
RuntimeSystem_roseConvertToString(T t) {
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return myStream.str(); //returns the string form of the stringstream object
};
#endif

char* 
RuntimeSystem_roseConvertIntToString(int t) {
  int size = sizeof(int);
  char* text = (char*)malloc(size+1);
  if (text)
    sprintf(text,"%d",t);
  printf("String converted from int : %s ",text); 
  return text;
}


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



/* -----------------------------------------------------------
 * create array and store its size
 * -----------------------------------------------------------*/
void
RuntimeSystem_roseCreateArray(char* name, int dimension, int stack, long int sizeA, long int sizeB, 
			       char* filename, char* line ){
  if (rtsi()->arrayDebug)
    printf( " >>> Called : roseCreateArray : %s dim %d - [%ld][%ld] file : %s line: %s\n",  
	    RuntimeSystem_findLastUnderscore(name),dimension,sizeA, sizeB, filename, line);

  if (dimension==1) {
    //rtsi()->arrays1D[name]=sizeA;
    printf("rtsi()->arrays1DEndIndex : %d rtsi()->maxArrays1DEndIndex: %d  \n",rtsi()->arrays1DEndIndex,rtsi()->maxArrays1DEndIndex);
    if (rtsi()->arrays1DEndIndex>=rtsi()->maxArrays1DEndIndex) {
      //increase the size of the array
      RuntimeSystem_increaseSizeArray1();
    }
    rtsi()->arrays1D[rtsi()->arrays1DEndIndex].name=name;
    rtsi()->arrays1D[rtsi()->arrays1DEndIndex].size1=sizeA;
    rtsi()->arrays1DEndIndex++;

      
    if (rtsi()->arrayDebug)
      printf( ".. Creating 1Dim array - size : %ld \n", sizeA);
  }
  else if (dimension==2) {
    // check if exist
    //std::map<char*, Array2D*>::const_iterator it = rtsi()->arrays2D.find(name);
    int pos = RuntimeSystem_findArrayName2D(name);
    //Array2D* array =NULL;
    //    if (it!=rtsi()->arrays2D.end()) {
    if (pos!=-1) {
      // array exists
      //array = it->second;
      //array = rtsi()->arrays2D[pos].array2d;
      //long int totalsize = array->size1;
      long int totalsize = rtsi()->arrays2D[pos].size1;
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
	//	array->allocate(sizeA,sizeB); //arr[sizeA][sizeB]
	rtsi()->arrays2D[pos].size2=sizeB;
      }
    } else {
      // new array
      //array = new Array2D(sizeA);// ptr [][] = malloc (20)   20 == totalsize
      //rtsi()->arrays2D[name]=array;
      if (rtsi()->arrays2DEndIndex>=rtsi()->maxArrays2DEndIndex) {
	//increase the size of the array
	RuntimeSystem_increaseSizeArray2();
      }
      rtsi()->arrays2D[rtsi()->arrays2DEndIndex].name=name;
      rtsi()->arrays2D[rtsi()->arrays2DEndIndex].size1=sizeA;
      rtsi()->arrays2D[rtsi()->arrays2DEndIndex].size2=0;
      //rtsi()->arrays2D[rtsi()->arrays2DEndIndex].array2d=array;
      rtsi()->arrays2DEndIndex++;
      

      if (rtsi()->arrayDebug)
	printf( ".. Creating 2Dim array - size : %ld \n", sizeA);
      if (sizeB!=-1) {
	// expand this stack array
	//for (int i=0;i<sizeA;++i)
	//  array->allocate(i,sizeB); //arr[i][sizeB]
	rtsi()->arrays2D[rtsi()->arrays2DEndIndex].size2=sizeB;
	if (rtsi()->arrayDebug)
	  printf( "..    Expanding 2Dim array - sizeA : %ld   sizeB: %ld \n", sizeA, sizeB);
      }
    }
  }
}

/* -----------------------------------------------------------
 * check if array is out of bounds
 * -----------------------------------------------------------*/
void
RuntimeSystem_roseArrayAccess(char* name, int posA, int posB, char* filename, char* line, char* stmtStr){
  if (rtsi()->arrayDebug) 
    printf( "    Called : roseArrayAccess : %s ... ", RuntimeSystem_findLastUnderscore(name));

  // check the stack if the variable is part of a function call
  char* mangl_name=RuntimeSystem_findVariablesOnStack(name);  
  if (mangl_name)
    name=mangl_name;

  //map<char*,int>::const_iterator it = rtsi()->arrays1D.find(name);
  int pos = RuntimeSystem_findArrayName1D(name);
  //  if (it!=rtsi()->arrays1D.end()) {
  if (pos!=-1) {
    //    int size = it->second;
    int size = rtsi()->arrays1D[pos].size1;
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

  //  cerr << " Done with 1Dim array" << endl;
  //std::map<char*, Array2D*>::const_iterator it2 = rtsi()->arrays2D.find(name);
  int pos2 = RuntimeSystem_findArrayName2D(name);
  //if (it2!=rtsi()->arrays2D.end()) {
  if (pos2!=-1) {
    //    Array2D* array = it2->second;
    //Array2D* array = rtsi()->arrays2D[pos2].array2d;
    //int sizeA = array->size1;
    //int sizeB = array->getSize(posA);
    int sizeA = rtsi()->arrays2D[pos2].size1;
    int sizeB = rtsi()->arrays2D[pos2].size2;
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
  //  cerr << " Done with 2Dim array" << endl;
  //  if (it==rtsi()->arrays1D.end() && it2==rtsi()->arrays2D.end()) {
  if (pos==-1 && pos2==-1) {
    printf("\n");
    printf( " >>> No such array was created. Can't access it. %s  line : %s \n" , filename, line);
    exit(1);
  }

}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************

void 
handleNormalFunctionCalls(char** args, int argsSize, char* filename, char* line, char* stmtStr) {
  if (rtsi()->funccallDebug)
    printf( "Runtimesystem :: normalFunctionCall \n");
  assert(argsSize>=2);
  char* mem1 = args[0];
  char* mem2 = args[1];
  assert(mem1);
  assert(mem2);
  int size = 0;
  if (argsSize>2)
    size = strtol(args[2],NULL,10);
  char* end1 = NULL;
  char* end2 = NULL;
  char *iter=NULL;
  char *iter2=NULL;
  for ( iter = mem1; *iter != '\0'; ++iter) {
    end1 = iter;
  }
  for (iter2 = mem2; *iter2 != '\0'; ++iter2) {
    end2 = iter2;
  }
  assert(end1);
  assert(end2);
    
  // check if string1 and string2 overlap. Dont allow memcopy on such
  int sizeMem1 = (end1-mem1)+1;
  int sizeMem2 = (end2-mem2)+1;
  if (rtsi()->funccallDebug)
    printf( " >>> FunctionCall : Checking mem1=<%s> (%d)  and mem2=<%s> (%d)  sizeOp: %d \n", 
	    mem1, sizeMem1, mem2 , sizeMem2 , size );
  if (end1 >= mem2 && mem1<=end2) {
    if (rtsi()->funccallDebug)
      printf( " >>>> Error : Memory regions overlap!   Size1: %d  Size2: %d\n",sizeMem1 , sizeMem2);
    RuntimeSystem_callExit(filename, line, (char*)"Memory regions overlap", stmtStr);  
  } else if (size>0 && (size>sizeMem1 || size>sizeMem2)) {
    // make sure that if the strings do not overlap, they are both smaller than the amount of chars to copy
    char* res1 = ((char*)"Invalid Operation,  operand1 size=");
    char* res2 = ((char*)"  operand2 size=");
    int sizeInt = 2*sizeof(int);
    char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ 1);
    sprintf(res,"%s%d%s%d",res1,sizeMem1,res2,sizeMem2);
    RuntimeSystem_callExit(filename, line, res, stmtStr);
  } else if (size==0) {
    // strcpy (mem1, mem2)
    // make sure that size of mem2 is <= mem1
    if (sizeMem1<sizeMem2) {
      char* res1 = ((char*)"Invalid Operation,  operand1 size=");
      char* res2 = ((char*)"  operand2 size=");
      char* res3 = ((char*)"  operand3 =");
      int sizeInt = 3*sizeof(int);
      char *res = (char*)malloc(strlen(res1) + strlen(res2) +sizeInt+ strlen(res3)+ 1);
      sprintf(res,"%s%d%s%d%s%d",res1,sizeMem1,res2,sizeMem2,res3,size);
      RuntimeSystem_callExit(filename, line, res, stmtStr);
    }
  }
  printf("No problem found!\n");
}


void 
RuntimeSystem_roseFunctionCall(int count, ...) {
  if (rtsi()->funccallDebug)
    printf( "Runtimesystem :: functionCall\n");
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  //std::vector<char*> args;
  char** args = (char**)malloc(sizeof(char*)*count+1);
  int posArgs=0;
  char* name = NULL;
  char* mangl_name = NULL;
  char* beforeStr = NULL;
  char* scope_name = NULL;
  char* filename = NULL;
  char* line=NULL;
  int before=0; //false
  char* stmtStr=NULL;
  //cerr << "arguments : " <<  count << endl;
  int i=0;
  for ( i=0;i<count;i++)    {
    char* val=  va_arg(vl,char*);
    //cerr << " ... " << val << endl;
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
      //      args.push_back(val);
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
		  strcmp(name ,"strcat")==0
		  )) {
    handleNormalFunctionCalls(args, posArgs, filename, line, stmtStr);
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

      //RuntimeVariables* var = new RuntimeVariables(name,mangl_name);
      //rtsi()->runtimeVariablesOnStack.push_back(var);
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



