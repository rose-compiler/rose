// A simpler case of this test code is in test2015_102.c.

struct uri;

int null_san_hook(struct uri *uri,unsigned int drive) 
   {
     return 0;
   }

// The bug is that the extern declaration is unparsed with the alias attribute. Unparsed as:
// extern int __null_san_hook(struct uri *uri,unsigned int drive) __attribute__((alias("null_san_hook")));
extern int __null_san_hook(struct uri *uri,unsigned int drive);
int __null_san_hook(struct uri *uri,unsigned int drive) __attribute__((alias("null_san_hook")));
