#include <wchar.h>

#define STR(x) 			L ## x

typedef wchar_t Char;

typedef int EditLine;

int map_bind(int);

const struct {
	const Char *name;
	int (*func)(int);
} cmds[] = {
	{ STR("bi\nnd"), map_bind	},
};

