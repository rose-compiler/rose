
#if 0
// Entries marks using "X" are attributes that are not allowed in function parameters.
X    align( # )
X    allocate(" segname ")
    appdomain
X    code_seg(" segname ")
    deprecated
    dllimport
    dllexport
X    jitintrinsic
X    naked
X    noalias
X    noinline
X    noreturn
X    nothrow
    novtable
    process
X    property( {get=get_func_name|,put=put_func_name})
    restrict
X    safebuffers
    selectany
    thread
    uuid(" ComObjectGUID ")
#endif


#if 0
void foobar_1( __declspec( align(4) ) int x )
   {
   }
#endif

#if 0
void foobar_2( __declspec( allocate("abc") ) int x )
   {
   }
#endif

void foobar_3( __declspec( appdomain ) int x )
   {
   }

void foobar_4( __declspec( selectany ) int x )
   {
   }

#if 0
void foobar_5( __declspec( code_seg("abc") ) int x )
   {
   }
#endif

#if 0
// This fails for EDG 4.7 (required EDG version 4.9).
void foobar_6( __declspec( deprecated ) int x )
   {
   }
#endif

void foobar_7( __declspec( dllimport ) int x )
   {
   }

void foobar_8( __declspec( dllexport ) int x )
   {
   }

void foobar_9( __declspec( selectany ) int x )
   {
   }

#if 0
void foobar_10( __declspec( jitintrinsic ) int x )
   {
   }
#endif

void foobar11( __declspec( selectany ) int x )
   {
   }

#if 0
void foobar_12( __declspec( naked ) int x )
   {
   }
#endif

#if 0
void foobar_13( __declspec( noalias ) int x )
   {
   }
#endif

#if 0
void foobar_14( __declspec( noinline ) int x )
   {
   }
#endif

#if 0
void foobar_15( __declspec( noreturn ) int x )
   {
   }
#endif

#if 0
void foobar_16( __declspec( nothrow ) int x )
   {
   }
#endif

void foobar_17( __declspec( novtable ) int x )
   {
   }

void foobar_18( __declspec( process ) int x )
   {
   }

#if 0
void foobar_19( __declspec( property( {get=get_func_name|,put=put_func_name}) ) int x )
   {
   }
#endif

void foobar_20( __declspec( restrict ) int* x )
   {
   }

#if 0
void foobar_21( __declspec( safebuffers ) int x )
   {
   }
#endif

void foobar_22( __declspec( selectany ) int x )
   {
   }

void foobar_23( __declspec( thread ) int x )
   {
   }

#if 0
// This fails for EDG 4.7 (required EDG version 4.9).
void foobar_24( __declspec( uuid(" ComObjectGUID ") ) int x )
   {
   }
#endif
