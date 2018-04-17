#ifndef ROSE_CONVERT_COBOL_H
#define ROSE_CONVERT_COBOL_H

extern FILE* rose_fd;
extern const struct cb_program* cobpt_program;
extern struct cobpt_base_list* cobpt_local_cache;

int rose_convert_cb_program(const struct cb_program* prog, struct cobpt_base_list* local_cache);
int rose_convert_cb_field(FILE* fd, struct cb_field* field);
int rose_convert_cb_literal(FILE* fd, struct cb_literal* lit);
int rose_convert_cb_label(FILE* fd, struct cb_label* label);
int rose_convert_cb_perform(FILE* fd, struct cb_perform* perf);
int rose_convert_cb_statement(FILE* fd, struct cb_statement* perf);
int rose_convert_cb_funcall(FILE* fd, struct cb_funcall* func);
int rose_convert_cb_param(FILE* fd, cb_tree x, int id);

#endif /* ROSE_CONVERT_COBOL_H */
