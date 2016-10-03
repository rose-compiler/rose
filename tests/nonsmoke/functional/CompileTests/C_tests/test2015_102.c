
int null_san_hook(unsigned int drive) 
   {
     return 0;
   }

// Unparsed code includes: "__attribute__((alias("null_san_hook")))"
//    extern int __null_san_hook(unsigned int drive) __attribute__((alias("null_san_hook")));
// Should be:
//    extern int __null_san_hook(unsigned int drive);
extern int __null_san_hook(unsigned int drive);

int __null_san_hook(unsigned int drive) __attribute__((alias("null_san_hook")));
