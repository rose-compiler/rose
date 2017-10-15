#ifndef ROSE_COBOL_H
#define ROSE_COBOL_H

extern FILE* rose_fd;

/* This struct is copied from codgen.c */
struct rose_field_list {
	struct rose_field_list	*next;
	struct cb_field		*f;
	cb_tree			x;
	const char		*curr_prog;
};

/* This struct is copied from codgen.c */
struct rose_base_list {
	struct rose_base_list	*next;
	struct cb_field		*f;
	const char		*curr_prog;
};



int rose_convert_cb_program(const struct cb_program* prog, struct rose_base_list* local_cache);
int rose_convert_cb_field(FILE* fd, struct cb_field* field);
int rose_convert_cb_literal(FILE* fd, struct cb_literal* lit);
int rose_convert_cb_label(FILE* fd, struct cb_label* label);
int rose_convert_cb_perform(FILE* fd, struct cb_perform* perf);
int rose_convert_cb_statement(FILE* fd, struct cb_statement* perf);
int rose_convert_cb_funcall(FILE* fd, struct cb_funcall* func);
int rose_convert_cb_param(FILE* fd, cb_tree x, int id);

#endif /* ROSE_COBOL_H */
