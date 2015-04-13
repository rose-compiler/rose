struct uri;

int null_san_hook(struct uri *uri,unsigned int drive) 
   {
     return 0;
   }

// extern int alt__null_san_hook(struct uri *uri,unsigned int drive) __attribute__((visibility("hidden")));
// int alt__null_san_hook(struct uri *uri,unsigned int drive) __attribute__((visibility("hidden"))) __attribute__((alias("null_san_hook")));

extern int __null_san_hook(struct uri *uri,unsigned int drive);
int __null_san_hook(struct uri *uri,unsigned int drive) __attribute__((alias("null_san_hook")));
