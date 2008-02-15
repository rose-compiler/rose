#if 0
compiling the attached file with ROSE I get the following error:
"/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prtpool.c",
line 9: error:
         identifier "__builtin_offsetof" is undefined
          __builtin_offsetof (PRJob, links);
          ^

"/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prtpool.c",
line 9: error:
         type name is not allowed
          __builtin_offsetof (PRJob, links);
                              ^

"/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prtpool.c",
line 9: error:
         identifier "links" is undefined
          __builtin_offsetof (PRJob, links);
                                     ^

Errors in EDG Processing: (edg_errorLevel > 3)
Aborted (core dumped) 

#endif

typedef struct PRJob PRJob;

struct PRJob {
	int			links;	
};

static void wstart()
{
         __builtin_offsetof (PRJob, links);
}


