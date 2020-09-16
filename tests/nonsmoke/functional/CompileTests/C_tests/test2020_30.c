// RC-62: Tristan (first two statements must be on the same line)

 // Original code: typedef void apr_HOOK__optional_t (void); typedef struct apr_LINK__optional_t {} apr_LINK__optional_t;
 // Generated code: typedef void apr_HOOK__optional_t();
 //                 typedef struct apr_LINK__optional_t apr_LINK__optional_t;
typedef void foobar (void); typedef struct X_t {} X_t;

void foo()
   {
     sizeof(X_t);
   }

