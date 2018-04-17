struct sudo_conf_paths {
    const char *pname;
    unsigned int pnamelen;
    const char *pval;
};

static struct sudo_conf_data 
   {
     struct sudo_conf_paths paths[2];
   } sudo_conf_data = 
        {
           {
              { "askpass", sizeof("askpass") - 1, ((void *)0) },
              { ((void *)0) }
           }
        };

