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

#include "CppRuntimeSystem/CppRuntimeSystem.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RuntimeSystem.h"

enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };



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
RuntimeSystem_roseRtedClose(
      const char* filename,
      const char* line,
      const char* lineTransformed
) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(
    SourcePosition( filename, atoi( line ),atoi( lineTransformed ) )
  );
	rs->doProgramExitChecks();

  // The runtime system would have exited if it had found an error
  rs->log( "Failed to discover error in RTED test\n" );
  exit( 1);
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
RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name,
				   const char* type, const char* basetype, size_t indirection_level,
			      unsigned long int address, long int size,
			      int ismalloc, long int mallocSize, const char* class_name, const char* filename, const char* line, const char* lineTransformed,
            int dimensionality, ...){


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));

	string type_name = type;
	if( type_name == "SgClassType" )
	    type_name = class_name;

  string base_type = basetype;
	if( base_type == "SgClassType" )
	    base_type = class_name;


  if( type_name == "SgArrayType" ) {
    // collect dimension information
    va_list vl;
    va_start( vl, dimensionality );

    size_t elements = 1;
    std::vector< size_t > dimensions;
    for( int i = 0; i < dimensionality; ++i ) {
      dimensions.push_back( va_arg( vl, size_t ));
      elements *= dimensions.back();
    }
    size_t base_size = size / elements;

    // recursively compute types bottom-up
    RsType* type = rs -> getTypeSystem() -> getTypeInfo( base_type );
    assert( type != NULL );

    size_t current_size = base_size;
    std::vector< size_t >::reverse_iterator itr = dimensions.rbegin();
    while( itr != dimensions.rend() ) {
      // however large the base type is, the type of the array is N times as
      // large
      current_size *= *itr;
      type = rs -> getTypeSystem() -> getArrayType( type, current_size );
      ++itr;
    }
    // create an array of arrays of arrays ... of basetype
    rs -> createArray( address, name, mangl_name, static_cast<RsArrayType*>(type) );
  } else if( type_name == "SgPointerType") {
    addr_type heap_address = *((addr_type*) address);
    rs -> createMemory( heap_address, mallocSize );
    rs -> registerPointerChange( address, heap_address );
  } else {
    cerr << "Unexpected Array Type: " << type << endl;
    exit( 1 );
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
			      unsigned long int base_address, unsigned long int address, long int size, 
            int read_write_mask, const char* line, const char* lineTransformed, const char* stmtStr){


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));

  RuntimeSystem_checkMemoryAccess( address, size, read_write_mask );

  if( read_write_mask & BoundsCheck ) {
    rs -> checkPointerDereference( base_address, address );
  }
}

// ***************************************** ARRAY FUNCTIONS *************************************


void RuntimeSystem_checkMemoryAccess( unsigned long int address, long int size, int read_write_mask ) {

	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

	if ( read_write_mask & Read )
	  rs->checkMemRead( address, size );
	if ( read_write_mask & Write )
    rs->checkMemWrite( address, size );
}






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


  rs->createArray((addr_type)mem,
                  "StringConstant",
                  "MangledStringConstant",
                  "SgTypeChar",size);
  rs->checkMemWrite( (addr_type) mem, size);
}

#define NUM_ARG( i )                                                          \
  strtol(                                                                     \
    i < argsSize - 1 && isdigit( args[ i + 1 ][ 0 ])                          \
      ? args[ i + 1 ] : args[ i ],                                            \
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
      (int) NUM_ARG( 4)
    );
  } else if ( 0 == strcmp("memmove", fname)) {
    rs->check_memmove(
      (void*) args[0], 
      (const void*) args[2], 
      (int) NUM_ARG( 4)
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

    // not handled (yet?)
    //  fclearerr
    //  feof
    //  ferror
    //  fgetpos
    //  fpos
    //  freopen
    //  fseek
    //  fsetpos
    //  ftell
    //  getchar
    //  putchar
    //  remove
    //  rename
    //  rewind
    //  scanf
    //  setbuf
    //  setvbuf
    //  sprintf
    //  sscanf
    //  tmpfile
    //  tmpnam
    //  ungetc
    //  vfprintf
    //  vprintf
    //  vsprintf
    if ( 0 == strcmp("fclose", fname)) {
      rs -> registerFileClose( file );
    } else if ( 0 == strcmp("fflush", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
    } else if ( 0 == strcmp("fgetc", fname)) {
      rs -> checkFileAccess( file, true /* is_read? */);
    } else if ( 0 == strcmp("fgets", fname)) {
      rs -> checkFileAccess( file, true /* is_read? */);
    } else if ( 0 == strcmp("fopen", fname)) {
      const char *filen = arg1;
      const char *mode = arg2;
      int openMode=-1;
      if (strcmp(mode,"r")==0)
        openMode=READ;
      if (strcmp(mode,"w")==0)
        openMode=WRITE;
      rs->registerFileOpen(file, filen, openMode);
    } else if ( 0 == strcmp("fprintf", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
    } else if ( 0 == strcmp("fputc", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
    } else if ( 0 == strcmp("fputs", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
    } else if ( 0 == strcmp("fread", fname)) {
      rs -> checkFileAccess( file, true /* is_read? */);
    } else if ( 0 == strcmp("fscanf", fname)) {
      rs -> checkFileAccess( file, true /* is_read? */);
    } else if ( 0 == strcmp("fwrite", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
    } else if ( 0 == strcmp("getc", fname)) {
      rs -> checkFileAccess( file, true /* is_read? */);
    } else if ( 0 == strcmp("putc", fname)) {
      rs -> checkFileAccess( file, false /* is_read? */);
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
  for ( i=0;i<count;i++)    {
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
              size_t indirection_level,
				      unsigned long int address,
				      unsigned int size,
				      int init,
				      const char* fOpen,
				      const char* class_name,
				      const char* filename, const char* line,
				      const char* lineTransformed) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint( SourcePosition(filename,atoi(line), atoi(lineTransformed)));


	string type_name = type;
    if (type_name == "SgClassType")
        type_name = class_name;

    string base_type = basetype;
    if (base_type == "SgClassType")
        base_type = class_name;


    // stack arrays are handled in create array, which is given the dimension
    // information
    if(type_name != "SgArrayType") {
        RsType * rsType;
        if(string(basetype) != "")
            rsType = rs->getTypeSystem()->getPointerType( base_type, indirection_level );
        else
            rsType = rs->getTypeSystem()->getTypeInfo(type_name);

        rs->createVariable(address,name,mangled_name,rsType);
    }


    if (1 == init)
    {
        // e.g. int x = 3
        // we should flag &x..&x+sizeof(x) as initialized
        rs->checkMemWrite(address, size);
    }
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
             size_t indirection_level,
			       unsigned long long address,
			       unsigned int size,
			       int ismalloc,
			       const char* filename,
			       const char* line,
			       const char* lineTransformed, const char* stmtStr) {


	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint( SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	rs->checkMemWrite(address,size);

  // This assumes roseInitVariable is called after the assignment has taken
  // place (otherwise we wouldn't get the new heap address).
  //
  // Note that we cannot call registerPointerChange until after the memory
  // creation is registered, which is done in roseCreateArray.
  if( ismalloc != 1 && 0 == strcmp( "SgPointerType", typeOfVar2 )) {
    addr_type heap_address = *((addr_type*) address);
    rs -> registerPointerChange( address, heap_address );
  }
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

  // TODO 1 djh: generate read_write_mask for var access, as with array access
  int read_write_mask = 1;
  RuntimeSystem_checkMemoryAccess( address, size, read_write_mask );
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
	  /*const char* typeC = */ va_arg(vl,const char*);
	  unsigned long long sizeC = va_arg(vl,unsigned long long);
	  //cerr << " Register Class : " << nameC << " Type: " << typeC << " size : " << sizeC << endl;
	  int i=0;


	  RsClassType * classType = new RsClassType(nameC,sizeC);
	  for ( i=5;i<count;i+=5)
	  {
		  string name = va_arg(vl,const char*);
		  string type = va_arg(vl,const char*);
      string base_type = va_arg(vl,const char*);
		  addr_type offset = va_arg(vl,unsigned long long);
      size_t size = va_arg(vl,size_t);

      RsType* t;
      if( type == "SgArrayType" ) { 
        t = RuntimeSystem::instance()->getTypeSystem()->getArrayType( base_type, size );
      } else if( type == "SgPointerType" ) {
        // TODO 1 djh: need indirection level
        t = RuntimeSystem::instance()->getTypeSystem()->getPointerType( base_type );
      } else {
        t = RuntimeSystem::instance()->getTypeSystem()->getTypeInfo( type );
      }
		  classType->addMember(name,t,(addr_type)offset);
		  //cerr << "Registering Member " << name << " of type " << type << " at offset " << offset << endl;
	  }
	  va_end(vl);


	  RuntimeSystem::instance()->getTypeSystem()->registerType(classType);
}

void
RuntimeSystem_roseFreeMemory(
      void* ptr,
      const char* filename,
      const char* line,
      const char* lineTransformed
) {

	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(
    SourcePosition( filename, atoi( line ),atoi( lineTransformed ) )
  );

	rs->freeMemory( (addr_type) ptr );
}


void
RuntimeSystem_roseReallocateMemory(
      void* ptr,
      unsigned long int size,
      const char* filename,
      const char* line,
      const char* lineTransformed
) {

	RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	rs->checkpoint(
    SourcePosition( filename, atoi( line ),atoi( lineTransformed ) )
  );

	rs->freeMemory( (addr_type) ptr );
  rs->createMemory( (addr_type) ptr, size);
}


// vim:sw=2 ts=2 et sta:

extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);

int main(int argc, char **argv, char ** envp)
{

    return RuntimeSystem_original_main(argc, argv, envp);
}
