struct globals {
	unsigned timeout;
	unsigned verbose;
	unsigned opts;
	char *user;
	char *pass;
};

#define G			(*ptr_to_globals)
#define timeout         (G.timeout  )

struct globals* ptr_to_globals;

char *send_mail_command(const char *fmt, const char *param);
