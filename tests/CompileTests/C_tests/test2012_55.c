
// if (strcmp(((const char *)name),((const char *)(sig -> name))) == 0) {
int strcmp(char*,char*);

int
ngx_os_signal_process(char *name)
   {
     char* name2;
     if (strcmp((const char *)name,(const char *)name2) == 0)
        {
        }

     return 1;
   }
