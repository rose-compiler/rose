#include <wchar.h>

#define STR(x) 			L ## x

// typedef wchar_t Char;
typedef wchar_t Char;

typedef int EditLine;

int map_bind(EditLine *, int, const Char **);
int terminal_echotc(EditLine *, int, const Char **);
int el_editmode(EditLine *, int, const Char **);
int hist_command(EditLine *, int, const Char **);
int terminal_telltc(EditLine *, int, const Char **);
int terminal_settc(EditLine *, int, const Char **);
int tty_stty(EditLine *, int, const Char **);

// int (*func)(EditLine *, int, const Char **);

const struct {
	const Char *name;
	int (*func)(EditLine *, int, const Char **);
} cmds[] = {
	{ STR("bind"),  	map_bind	},
	{ STR("echotc"),	terminal_echotc	},
	{ STR("edit"),  	el_editmode	},
	{ STR("history"),	hist_command	},
	{ STR("telltc"),	terminal_telltc	},
	{ STR("settc"),	        terminal_settc	},
	{ STR("setty"),	        tty_stty	},
	{ NULL,		        NULL		}
};

