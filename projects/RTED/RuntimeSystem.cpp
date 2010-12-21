// vim:sw=2 ts=2 et sta:
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

#ifdef ROSE_WITH_ROSEQT
#include "CppRuntimeSystem/DebuggerQt/RtedDebug.h"
#endif


/**********************************************************
 *  Convert to string
 *********************************************************/
  template<typename T>
  std::string ToString(T t){
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return myStream.str(); //returns the string form of the stringstream object
 }

template<typename T>
std::string HexToString(T t){
  std::ostringstream myStream; //creates an ostringstream object
  myStream <<  std::hex <<  t ;
  return myStream.str(); //returns the string form of the stringstream object
}

std::string AsString(MemoryAddress addr, Indirections desc)
{
	if (desc.shared_mask == MASK_SHARED) return " shared-address ";

  std::ostringstream myStream; //creates an ostringstream object

  myStream << std::hex << t ;
  return myStream.str(); //returns the string form of the stringstream object
}

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
rted_RtedClose(char* from) {

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

  rs->doProgramExitChecks();

  std::string stdfrom = string(from);
  // The runtime system would have exited if it had found an error
  rs->log( "Failed to discover error in RTED test. Origin : "+stdfrom+"\n" );
  exit( 1 );
}


// ***************************************** ARRAY FUNCTIONS *************************************

struct ArrayTypeComputer
{
	typedef std::pair<RsArrayType*, size_t> Operand;

	TypeSystem& ts;

  explicit
  ArrayTypeComputer(TypeSystem& tsys)
	: ts(tsys)
	{}

	RsArrayType* operator()(Operand prev, size_t dimSize)
	{
		size_t newsz = prev.second * dimSize;

		return Operand(ts->getArrayType(prev.first, newsz), newsz);
	}
};


// TODO 3 djh: doxygenify
/*
 * Returns an RsArrayType* whose dimensionality dimensions are given by dimDesc and
 * whose base non-array type is base_type.
 */
RsArrayType* RuntimeSystem_getRsArrayType( const size_t* dimDesc,
                                           size_t size,
																					 const std::string& base_type
																				 )
{
  assert( dimDesc != NULL && *dimdesc > 0 );

  TypeSystem* ts = RuntimeSystem_getRuntimeSystem() -> getTypeSystem();

  // collect dimension information
	const size_t*       first = dimDesc + 1;
	const size_t*       last = dimDesc + *dimDesc;  // points to the last element (not one past!)
	                                                //   which is the first element being processed
  size_t              no_elems = std::accumulate(first, last+1, static_cast<size_t>(1), std::multiplies<size_t>());
	RsType*             type = ts -> getTypeInfo( base_type );
	size_t              elem_size = (size / no_elem);
	size_t              last_sz = elem_size * (*last);
	RsArrayType*        res = ts -> getArrayType( type, last_sz );

	return std::accumulate( reverse_iterator(last),
	                        reverse_iterator(first),
													ArrayTypeComputer::Operand(res, last_sz),
													ArrayTypeComputer(*ts)
												).first;
}

RsType* RuntimeSystem_getRsType( std::string type,
				                         std::string base_type,
				                         std::string class_name,
				                         size_t indirection_level
															 )
{
  if( type == "SgClassType" )
    type = class_name;
  else if( base_type == "SgClassType" ) {
    base_type = class_name;
    assert( base_type != "" );
  }
  assert( type != "" );

  RsType* tt = NULL;
  if( type == "SgPointerType" ) {
    RsType* bt = NULL;
    assert( indirection_level > 0 );

    bt = RuntimeSystem::instance() -> getTypeSystem()
        -> getTypeInfo( base_type );

    if( bt == NULL ) {
      // Create a type stub.  This will result in errors later if it is not
      // fixed up by a subsequent registration (i.e., of that type).
      bt = new RsClassType( base_type, 0, false );
      RuntimeSystem::instance()->getTypeSystem()->registerType( bt );
    }

    assert( bt != NULL );

    tt= RuntimeSystem::instance() -> getTypeSystem()
      -> getPointerType( bt, indirection_level );

    assert( tt );
  } else {
    tt= RuntimeSystem::instance() -> getTypeSystem()
      -> getTypeInfo( type );

    if( tt == NULL ) {
      // Create a type stub.  This will result in errors later if it is not
      // fixed up by a subsequent registration (i.e., of that type).
      tt = new RsClassType( type, 0, false );
      RuntimeSystem::instance()->getTypeSystem()->registerType( tt );
    }

    assert( tt );
  }
  return tt;
}

RsType* RuntimeSystem_getRsType(
				std::string type,
				std::string base_type,
				size_t indirection_level) {

  // in this case we don't do any class name resolution, presumably because it
  // was done in the transformation
  return RuntimeSystem_getRsType( type, base_type, "", indirection_level );
}




#if OBSOLETE_CODE

// FIXME 3: This is not threadsafe.  At the moment, because createVariable and
// createArray are called for stack and pointer arrays and because
// createVariable can't call rs -> createArray without the dimension
// information, this hack exists.  It will not be necessary after the
// transformation is refactored.
bool initialize_next_array = false;
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
rted_CreateHeap(const char* name, const char* mangl_name,
			      const char* type, const char* basetype, size_t indirection_level,
			      addr_type address, long int size,
			      long int mallocSize, int fromMalloc, const char* class_name,
			      const char* filename, size_t line, size_t lineTransformed,
			      const size_t* dimDesc){


  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

    string type_name = type;
  string base_type = basetype;
  if( base_type == "SgClassType" )
    base_type = class_name;


  if( type_name == "SgArrayType" ) {
    // Aug 6 : TODO : move this to createVariable
    RsArrayType* type = RuntimeSystem_getRsArrayType( dimDesc, size, base_type );

    rs -> createArray( address, name, mangl_name, type );

    if( initialize_next_array ) {
      rs -> checkMemWrite( address, size );
      initialize_next_array = false;
    }
  } else if( type_name == "SgPointerType") {
    addr_type heap_address = *((addr_type*) address);
    cerr << " registering heap   type:" << type << "  basetype:"<<basetype<<
    		"  class_name:" <<class_name<<"  indirection_level:"<<ToString(indirection_level)<<
    		"  address:"<<HexToString(heap_address) <<"  malloc size:"<<ToString(mallocSize)<<endl;

    RsPointerType* rs_type
      = static_cast< RsPointerType* >(
          RuntimeSystem_getRsType(
							type,
							basetype,
							class_name,
							indirection_level ));
    RsClassType* class_type
      = dynamic_cast< RsClassType* >( rs_type -> getBaseType() );
    // A class might have had its memory allocation registered in the
    // constructor.  If there was no explicit constructor, however, we still
    // have to allocate the memory here.
    if(   !class_type
          || rs -> getMemManager() -> findContainingMem( heap_address) == NULL ) {
      // FIXME 2: This won't handle the unlikely case of a C++ object being
      // allocated via malloc and then freed with delete.
      //
      // object memory allocation is handled in the constructor
      bool was_from_malloc = ( fromMalloc == 1 );
      rs -> createMemory( heap_address, mallocSize, false, was_from_malloc );
    }
    rs -> registerPointerChange(
				address,
				heap_address,
        rs_type,
				false,  // checkPtrMove? no, pointer may change regions
				true    // checkMemLeak? yes
				);
  } else {
    cerr << "Unexpected Array Type: " << type << endl;
    exit( 1 );
  }
}

#endif /* OBSOLETE_CODE */

/*********************************************************
 * This function is called when an array is created
 *   (the shared memory version was copied form rted_CreateHeap)
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
rted_CreateHeapArr( const char* name,
									  const char* mangl_name,
									  const char* type,
										const size_t* dimDescr,
										size_t totalsize,
									  const char* basetype,
										const char* class_name,
									  MemoryAddress address,
									  const char* filename,
									  size_t line,
									  size_t lineTransformed
									)
{
	assert(std::string("SgArrayType") == type);

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs->checkpoint(SourcePosition(filename, line, lineTransformed));

  string base_type = basetype;
  if( base_type == "SgClassType" )
    base_type = class_name;

	// Aug 6 : TODO : move this to createVariable
	RsArrayType* type = RuntimeSystem_getRsArrayType(dimDescr, totalsize, base_type);

	rs -> createArray( address, name, mangl_name, type );

	if( initialize_next_array ) {
		rs -> checkMemWrite( address, size );
		initialize_next_array = false;
	}
}

void
rted_CreateHeapPtr( const char* type,
									  const char* basetype,
									  MemoryAddress address,
										Indirections desc,
									  size_t mallocSize,
									  int fromMalloc,
									  const char* class_name,
									  const char* filename,
									  size_t line,
									  size_t lineTransformed
									)
{
	assert(std::string("SgPointerType") == type);

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs->checkpoint(SourcePosition(filename, line, lineTransformed));

  string base_type = basetype;
  if( base_type == "SgClassType" )
    base_type = class_name;

	MemoryAddress  heap_address = deref(address, desc);
	RsType*        rs_type = RuntimeSystem_getRsType(type, basetype, class_name, desc));
	RsPointerType* rs_ptr_type = static_cast< RsPointerType* >(rs_type);
	RsClassType*   class_type = dynamic_cast< RsClassType* >(rs_ptr_type->getBaseType());

	std::cerr << " registering heap type:" << type
						<< "   basetype: " << basetype
						<< "   class_name: " << class_name
						<< "   indirection_level: " << ToString(desc.level)
						<< "   indirection_level: " << ToString(desc.level)
						<< "   address: " << AsString(heap_address, remove(desc, 1))
						<< "   malloc size: " << ToString(mallocSize)
						<< std::endl;

	// A class might have had its memory allocation registered in the
	// constructor.  If there was no explicit constructor, however, we still
	// have to allocate the memory here.
	if(!class_type || rs->getMemManager()->findContainingMem(heap_address) == NULL)
	{
		// FIXME 2: This won't handle the unlikely case of a C++ object being
		// allocated via malloc and then freed with delete.
		//
		// object memory allocation is handled in the constructor
		bool was_from_malloc = ( fromMalloc == 1 );
		rs -> createMemory( heap_address, mallocSize, false, was_from_malloc );
	}

	rs -> registerPointerChange(
			address,
			heap_address,
			rs_ptr_type,
			false,  // checkPtrMove? no, pointer may change regions
			true    // checkMemLeak? yes
			);
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
rted_AccessHeap( const char* filename,
			           MemoryAddress base_address,
								 MemoryAddress address,
								 size_t size,
			           int read_write_mask,
								 size_t line,
								 size_t lineTransformed
							 )
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs->checkpoint(SourcePosition(filename, line, lineTransformed));

	RuntimeSystem_checkMemoryAccess(address, size, read_write_mask);

  if( read_write_mask & BoundsCheck ) {
    rs->getMemManager()->checkIfSameChunk(base_address, address, size);
  }
}

// ***************************************** ARRAY FUNCTIONS *************************************


void RuntimeSystem_checkMemoryAccess( MemoryAddress address, size_t size, int read_write_mask)
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

  if (read_write_mask & Read)
    rs->checkMemRead( address, size );

  if (read_write_mask & Write)
    rs->checkMemWrite( address, size );
}






// ***************************************** FUNCTION CALL *************************************

std::vector< RsType* >
rted_GatherTypes( int type_count, va_list vl ) {

  std::vector< RsType* > types;

  for( int i = 0; i < type_count; ++i ) {
    const char* type_name = va_arg( vl, const char* );
    const char* base_type_name = va_arg( vl, const char* );
    int indirection_level = va_arg( vl, int );

    types.push_back( RuntimeSystem_getRsType(
                                             type_name, base_type_name, indirection_level ));
  }

  return types;
}

void rted_AssertFunctionSignature(
                                               const char* filename, size_t line, size_t lineTransformed,
					       const char* name, int type_count, ... ) {

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

    va_list vl;
  va_start( vl, type_count );

  std::vector< RsType* > types
    = rted_GatherTypes( type_count, vl );

  rs -> expectFunctionSignature( name, types );

  va_end( vl );
}

void rted_ConfirmFunctionSignature(
						const char* name, int type_count, ... ) {

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

  va_list vl;
  va_start( vl, type_count );

  std::vector< RsType* > types
    = rted_GatherTypes( type_count, vl );

  rs -> confirmFunctionSignature( name, types );

  va_end( vl );
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



void RuntimeSystem_ensure_allocated_and_initialized( const void* mem, size_t size)
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();

  // We trust that anything allocated is properly initialized -- we're not
  // working around a string constant so there's no need for us to do anything.
  if(rs->getMemManager()->findContainingMem( (addr_type) mem) != NULL)
    return;


  rs->createArray((addr_type)mem,
                  "StringConstant",
                  "MangledStringConstant",
                  "SgTypeChar",size);
  rs->checkMemWrite( (addr_type) mem, size);
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
inline
void handle_string_constant(const char** args, size_t i)
{
  if (isdigit(args[ i + 1 ][0]))
	{
    RuntimeSystem_ensure_allocated_and_initialized(args[ i ], strtol( args[ i + 1 ], NULL, 10));
	}
}

inline
int num_arg(const char** args, size_t argsSize, size_t i)
{
	assert (argsize > 0);

	const size_t argpos = i;

	if (i < argsSize - 1 && isdigit( args[ i + 1 ][ 0 ])) ++argpos;

  return strtol(args[argpos], NULL, 10);
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
                                         const char* filename, size_t line,
					 size_t lineTransformed,
					 const char* stmtStr, const char* leftHandSideVar) {


  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

    // FIXME 2: The current transformation outsputs num (for, e.g. strncat) as
    //    (expr), (size in str)
    // but size in str is off by one -- it includes the null terminator, but
    // should not.

    if( 0 == strcmp("memcpy", fname)) {
      rs->check_memcpy((void*) args[0], (const void*) args[2], num_arg(args, argSize, 4));
    } else if ( 0 == strcmp("memmove", fname)) {
      rs->check_memmove( (void*) args[0], (const void*) args[2], num_arg(args, argSize, 4));
    } else if ( 0 == strcmp("strcpy", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcpy((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncpy", fname)) {
      rs->check_strncpy((char*) args[0], (const char*) args[2], num_arg(args, argSize, 4));
    } else if ( 0 == strcmp("strcat", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcat((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncat", fname)) {
      rs->check_strncat( (char*) args[0], (const char*) args[2], num_arg(args, argSize, 4));
    } else if ( 0 == strcmp("strchr", fname)) {
      rs->check_strchr((const char*) args[0], num_arg(args, argSize, 2));
    } else if ( 0 == strcmp("strpbrk", fname)) {
      rs->check_strpbrk((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strspn", fname)) {
      rs->check_strspn( (const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strstr", fname)) {
      rs->check_strstr((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strlen", fname)) {
      rs->check_strlen((const char*) args[0]);
    } else {
      std::cerr << "Function " << fname << " not yet handled." << std::endl;
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
rted_IOFunctionCall(const char* fname,
                                 const char* filename, size_t line, size_t lineTransformed,
				 const char* stmtStr, const char* leftHandSideVar, void* file,
				 const char* arg1, const char* arg2) {

  //fixme - we need to create a new function call that
  // will have FILE* as parameter
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

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
      rs -> registerFileClose( (FILE*)file );
    } else if ( 0 == strcmp("fflush", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fgetc", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fgets", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fopen", fname)) {
      const char *filen = arg1;
      const char *mode = arg2;
      int openMode=-1;
      if (strcmp(mode,"r")==0)
        openMode=READ;
      if (strcmp(mode,"w")==0)
        openMode=WRITE;
      rs->registerFileOpen((FILE*)file, filen, openMode);
    } else if ( 0 == strcmp("fprintf", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputc", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputs", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fread", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fscanf", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fwrite", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("getc", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("putc", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    }
     else if ( 0 == strcmp("std::fstream", fname)) {
#if 1
       if (0 == strcmp(arg1,"r"))
	 rs -> checkFileAccess((std::fstream&) file,true /* is_read? */);
       else
	 rs -> checkFileAccess((std::fstream&) file,false /* is_read? */);
#else
         cerr <<" fstream unhandled . " << arg1 << endl;
         abort();
#endif
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
rted_FunctionCall(int count, ...) {
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  const char** args = (const char**)malloc(sizeof(const char*)*count+1);
  int posArgs=0;
  const char* name = NULL;
  const char* filename = NULL;
  size_t line=NULL;
  size_t lineTransf=NULL;
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


  //if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
  // if the string name is one of the above, we handle it specially
  RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  // } else if (RuntimeSystem_isFileIOFunctionCall(name)==1) {
  // this will be replaced by a direct call
  //RuntimeSystem_handleIOFunctionCall(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar, NULL);
  //} else {
  //printMessage("Unknown Function call to RuntimeSystem!\n");
  //  exit(1);
  //}
}



// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************

void rted_EnterScope(const char* name) {

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> beginScope( name );

}

void rted_ExitScope( const char* filename, size_t line, size_t lineTransformed, const char* stmtStr) {

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));
    rs -> endScope();
}




// ***************************************** SCOPE HANDLING *************************************


// ***************************************** VARIABLE DECLARATIONS *************************************
/*********************************************************
 * This function tells the runtime system that a variable is created
 * we store the type of the variable and whether it has been intialized
 ********************************************************/
int rted_CreateVariable( const char* name,
												 const char* mangled_name,
												 const char* type,
												 const char* basetype,
												 Indirections indirection_level,
												 MemoryAddress address,
												 size_t size,
												 int init,
												 const char* class_name,
												 const char* filename,
												 size_t line,
												 size_t lineTransformed
						           )
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

	string type_name = type;
  assert( type_name != "" );

  // stack arrays are handled in create array, which is given the dimension
  // information
  if(type_name != "SgArrayType") {
    RsType * rsType = RuntimeSystem_getRsType(
					      type_name,
					      basetype,
					      class_name,
					      indirection_level
					      );
    // tps : (09/04/2009) : It seems to be allowed for the type to be NULL
    //  in order to register new types
    // if (rsType==NULL) cerr <<" type: " << type_name << " unknown " << endl;

		assert(rsType);
    rs->createVariable(address,name,mangled_name,rsType);
  }


  if ( 1 == init ) {
    // e.g. int x = 3
    // we should flag &x..&x+sizeof(x) as initialized

    if( type_name == "SgArrayType" )
      initialize_next_array = true;
    else
      rs -> checkMemWrite( address, size );
  }

  return 0;
}


void rted_CreateObject( const char* type_name,
		 									  const char* base_type,
											  Indirections indirection_level,
											  MemoryAddress address,
											  const char* filename,
											  size_t line,
											  size_t lineTransformed
											)
{
	assert(indirection_level.shared_mask == 0); // no objects in UPC

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

  RsClassType * rs_type = static_cast< RsClassType* >(
        RuntimeSystem_getRsType(
              type_name,
              base_type,
              indirection_level ));
  assert( rs_type );

  rs -> createObject( address, rs_type );
}


/*********************************************************
 * For a given variable name, check if it is present
 * in the pool of variables created and return mangled_name
 ********************************************************/
void rted_InitVariable( const char* type,
			                  const char* base_type,
			                  const char* class_name,
			                  MemoryAddress address,
												size_t indirection_level,
			                  size_t size,
			                  int ismalloc,
			                  int pointer_changed,
			                  const char* filename,
			                  size_t line,
			                  size_t lineTransformed
											)
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

  std::string message = "   Init Var at address:  "+HexToString(address)+"  type:"
    +type+ "   size: " + ToString(size);
  rs->printMessage(message);


  RsType* rs_type = RuntimeSystem_getRsType(type, base_type, class_name, indirection_level);
  rs -> checkMemWrite( address, size, rs_type );

  // This assumes roseInitVariable is called after the assignment has taken
  // place (otherwise we wouldn't get the new heap address).
  //
  // Note that we cannot call registerPointerChange until after the memory
  // creation is registered, which is done in roseCreateHeap.
  //cerr << "============ Possible pointer change : ismalloc: "<<ismalloc<<
  // "   pointer_changed:" << pointer_changed << " pointer_type:" <<
  //  type<<endl;

  if(  ismalloc != 1
	  && pointer_changed == 1
	  && 0 == strcmp( "SgPointerType", type)
		)
	{
    MemoryAddress heap_address = deref(address);
    rs -> registerPointerChange( address, heap_address, rs_type, false, true );
  }
}

// we want to catch errors like the following:
//    int x[2] = { 0, 1 };
//    int y;
//    int *p = &x[1];
//    p++;
//
//    int q = *p;
void
rted_MovePointer( const char* type,
			            const char* base_type,
									MemoryAddress address,
									size_t indirection_level,
									const char* class_name,
									const char* filename,
									size_t line,
									size_t lineTransformed
								)
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

  MemoryAddress heap_address = deref(address);
  RsType* rs_type =
    RuntimeSystem_getRsType(
			    type,
			    base_type,
			    class_name,
			    indirection_level
			    );
  rs -> registerPointerChange( address, heap_address, rs_type, true, false );
}


/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void rted_AccessVariable( MemoryAddress address,
													size_t size,
													MemoryAddress write_address,
													size_t write_size,
													int read_write_mask,
													const char* filename,
													size_t line,
													size_t lineTransformed
				                )
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs->checkpoint(SourcePosition(filename, line, lineTransformed));

	RuntimeSystem_checkMemoryAccess(address, size, read_write_mask & Read);
  RuntimeSystem_checkMemoryAccess(write_address, write_size, read_write_mask & Write);
}

// ***************************************** VARIABLE DECLARATIONS *************************************


/*********************************************************
 * Convert an integer to const char*
 ********************************************************/
const char*
rted_ConvertIntToString(int t) {
  int size = sizeof(int);
  char* text = (char*)malloc(size+1);
  if (text)
    sprintf(text,"%d",t);
  //printMessage("String converted from int : %s ",text);
  return text;
}

// A simple way for users to manually set checkpoints
void
rted_Checkpoint(const char* filename, size_t line, size_t lineTransformed) {
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));
}


void
rted_RegisterTypeCall(int count, ...) {
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  const char* filename = va_arg(vl,const char*);
  size_t line = va_arg(vl,const char*);
  size_t lineTransformed = va_arg(vl,const char*);

  const char* nameC = va_arg(vl,const char*);
  /*const char* typeC = */ va_arg(vl,const char*);
  const char* isUnionType = va_arg(vl,const char*);
  bool isUnion=false;
  if (*isUnionType=='1') isUnion=true;
  addr_type sizeC = va_arg(vl,addr_type);
  //cerr << " +++++ Register Class : " << nameC << " size : " << sizeC << "  isUnion : " << isUnion << endl;
  int i=0;

  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));


  RsClassType * classType = static_cast< RsClassType* >(
      RuntimeSystem::instance() -> getTypeSystem() -> getTypeInfo( nameC ));

  if( NULL == classType )  {
    // no stub has been registered
    classType = new RsClassType(nameC,sizeC, isUnion);
    RuntimeSystem::instance()->getTypeSystem()->registerType(classType);
  } else {
    // a stub was registered, we have to fix up its properties
    classType -> setByteSize( sizeC );
    classType -> setUnionType( isUnion );
  }

  for ( i=7;i<count;i+=6)
    {
      string name = va_arg(vl,const char*);
      string type = va_arg(vl,const char*);
      string base_type = va_arg(vl,const char*);
      int indirection_level = va_arg( vl, int );
      addr_type offset = va_arg(vl,addr_type);
      size_t size = va_arg(vl,size_t);

      RsType* t;
      if( type == "SgArrayType" ) {
        unsigned int dimensionality = va_arg( vl, unsigned int );
        i += dimensionality + 1;
        t = RuntimeSystem_getRsArrayType( &vl, dimensionality, size, base_type );
      } else {
        t = RuntimeSystem_getRsType( type, base_type, "", indirection_level );
      }

        std::string message = "   Register class-member:  "+name+"  offset:"
          +HexToString(offset)+ "   size: " + ToString(size);
        rs->printMessage(message);

      classType->addMember(name,t,(addr_type)offset);
      //cerr << "Registering Member " << name << " of type " << type << " at offset " << offset << endl;
    }
  va_end(vl);
}

void
rted_FreeMemory( void* ptr,
                 int fromMalloc,
			           const char* filename,
			           size_t line,
			           size_t lineTransformed
			         )
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

    bool expected_to_free_memory_from_malloc = ( fromMalloc == 1 );
    rs->freeMemory( (addr_type) ptr, false, expected_to_free_memory_from_malloc );
}


void
rted_ReallocateMemory( void* ptr,
				               size_t size,
				               const char* filename,
				               size_t line,
				               size_t lineTransformed
				             )
{
  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

	rs->freeMemory( (addr_type) ptr,
		false,  // not freeing stack memory
		true    // we do expect to free memory that came from malloc (i.e. not new)
	);

	rs->createMemory(
		(addr_type) ptr, size,
		false,    // not on stack
		true      // was from malloc (i.e. don't use delete on this memory)
	);
}



void
rted_CheckIfThisNULL( void* thisExp,
		                  const char* filename,
											size_t line,
		                  size_t lineTransformed
										)
{
	//cerr <<" Asserting that thisExp is != NULL " << endl;
	  RuntimeSystem * rs = RuntimeSystem_getRuntimeSystem();
	  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));
	  rs->checkIfThisisNULL(thisExp);
}


/*********************************************************
 * Convert an integer to const char*
 ********************************************************/



extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);

#if 0
int main(int argc, char **argv, char ** envp) {

  int exit_code = RuntimeSystem_original_main(argc, argv, envp);
  rted_RtedClose((char*)"RuntimeSystem.cpp:main");

  return exit_code;
}

#endif
