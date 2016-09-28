#define SIGTERM 1
#define SIGINT  1
#define SIGHUP  1
#define SIGTTIN 1
#define SIGTTOU 1
#define SIGTSTP 1

#define ENABLE_HUSH_JOB 1

enum {
	SPECIAL_INTERACTIVE_SIGS = 0
		| (1 << SIGTERM)
		| (1 << SIGINT)
		| (1 << SIGHUP)
		,
	SPECIAL_JOBSTOP_SIGS = 0
#if ENABLE_HUSH_JOB
		| (1 << SIGTTIN)
		| (1 << SIGTTOU)
		| (1 << SIGTSTP)
#endif
		,
};

