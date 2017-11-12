#include <stdio.h>

#include "gnucobpt.h"
#include "rose_convert_cobol.h"

#define DUMP_PARSE_TREE 0

FILE* rose_fd;

const struct cb_program* cobpt_program;
struct cobpt_base_list* cobpt_local_cache;

int cobpt_convert_cb_program(const struct cb_program* prog, struct cobpt_base_list* local_cache)
{
   int status = 0;

   cobpt_program = prog;
   cobpt_local_cache = local_cache;

#if DUMP_PARSE_TREE
   status = rose_convert_cb_program(prog, local_cache);
#endif

   return status;
}

int rose_convert_cb_program(const struct cb_program* prog, struct cobpt_base_list* local_cache)
{
   cb_tree tree;
   struct cobpt_base_list *field;
   FILE* fd = rose_fd;

   if (rose_fd == NULL)
   {
      fd = rose_fd = stdout;
   }

   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_program\n%s\n", prog->orig_program_id);
   fprintf(fd, "|\n");

   fprintf(fd, "next_program: %p\n", prog->next_program);

   if (cb_list_length (prog->entry_list)) fprintf(fd, "entry_list(%d)\n", cb_list_length (prog->entry_list));
   if (cb_list_length (prog->exec_list))  fprintf(fd, "entry_list(%d)\n", cb_list_length (prog->exec_list));
   if (prog->flag_main)                   fprintf(fd, "flag_main: %d\n", prog->flag_main);

   if (prog->source_name     ) fprintf(fd, "source_name: %s\n",      prog->source_name);
   if (prog->nested_prog_list) fprintf(fd, "nested_prog_list: %p\n", prog->nested_prog_list);
   if (prog->common_prog_list) fprintf(fd, "common_prog_list: %p\n", prog->common_prog_list);

 /* The local C header file from code generation */
 /* if (prog->local_include) printf("local_include: %p\n", prog->local_include); */

   if (cb_list_length (prog->file_list)) fprintf(fd, "file_list: %d\n", cb_list_length (prog->file_list));
   if (cb_list_length (prog->cd_list)) fprintf(fd, "cd_list: %d\n", cb_list_length (prog->cd_list));
   if (cb_list_length (prog->label_list)) fprintf(fd, "label_list: %d\n", cb_list_length (prog->label_list));
   if (cb_list_length (prog->reference_list)) fprintf(fd, "reference_list: %d\n", cb_list_length (prog->reference_list));
   if (cb_list_length (prog->alphabet_name_list)) fprintf(fd, "alphabet_name_list: %d\n", cb_list_length (prog->alphabet_name_list));
   if (cb_list_length (prog->symbolic_char_list)) fprintf(fd, "symbolic_char_list: %d\n", cb_list_length (prog->symbolic_char_list));
   if (cb_list_length (prog->class_name_list)) fprintf(fd, "class_name_list: %d\n", cb_list_length (prog->class_name_list));
   if (cb_list_length (prog->parameter_list)) fprintf(fd, "parameter_list: %d\n", cb_list_length (prog->parameter_list));
   if (cb_list_length (prog->locale_list)) fprintf(fd, "locale_list: %d\n", cb_list_length (prog->locale_list));
   if (cb_list_length (prog->global_list)) fprintf(fd, "global_list: %d\n", cb_list_length (prog->global_list));
   if (cb_list_length (prog->report_list)) fprintf(fd, "report_list: %d\n", cb_list_length (prog->report_list));
   if (cb_list_length (prog->alter_list)) fprintf(fd, "alter_list: %d\n", cb_list_length (prog->alter_list));
   if (cb_list_length (prog->debug_list)) fprintf(fd, "debug_list: %d\n", cb_list_length (prog->debug_list));
   if (cb_list_length (prog->mnemonic_spec_list)) fprintf(fd, "mnemonic_spec_list: %d\n", cb_list_length (prog->mnemonic_spec_list));
   if (cb_list_length (prog->class_spec_list)) fprintf(fd, "class_spec_list: %d\n", cb_list_length (prog->class_spec_list));
   if (cb_list_length (prog->interface_spec_list)) fprintf(fd, "interface_spec_list: %d\n", cb_list_length (prog->interface_spec_list));
   if (cb_list_length (prog->function_spec_list)) fprintf(fd, "function_spec_list: %d\n", cb_list_length (prog->function_spec_list));
   if (cb_list_length (prog->user_spec_list)) fprintf(fd, "user_spec_list: %d\n", cb_list_length (prog->user_spec_list));
   if (cb_list_length (prog->program_spec_list)) fprintf(fd, "program_spec_list: %d\n", cb_list_length (prog->program_spec_list));
   if (cb_list_length (prog->property_spec_list)) fprintf(fd, "property_spec_list: %d\n", cb_list_length (prog->property_spec_list));

   fprintf(fd, "\n--------------\n");
   if (cb_list_length (prog->entry_list))
   {
      fprintf(fd, "entry_list(%d)\n", cb_list_length (prog->entry_list));
   }

   /* variable declarations */
   for (field = local_cache; field; field = field->next) {
      rose_convert_cb_field(fd, field->f);
   }

   /* statements */
   fprintf(fd, "\n--------------\n");
   if (cb_list_length (prog->exec_list))
   {
      fprintf(fd, "exec_list(%d)\n", cb_list_length (prog->exec_list));
   }

   for (tree = prog->exec_list; tree; tree = CB_CHAIN (tree)) {
   // struct cb_statement *p;
      cb_tree tt = CB_VALUE (tree);
      printf("\nEXEC: tag = %d", tt->tag);

    //    const char * entry_name = CB_LABEL (CB_PURPOSE (tree))->name;
    //    printf("    entry name = %s\n", entry_name);

      switch (tt->tag) {
      case CB_TAG_LABEL: /* 18 */
         rose_convert_cb_label(fd, CB_LABEL (tt));
         break;
      case CB_TAG_PERFORM: /* 25 */
         rose_convert_cb_perform(fd, CB_PERFORM (tt));
         break;
      case CB_TAG_STATEMENT: /* 26 */
         rose_convert_cb_statement(fd, CB_STATEMENT (tt));
         break;
      default:
         break;
      }
   }

   return 0;
}

int rose_convert_cb_field(FILE* fd, struct cb_field* field)
{
   cb_tree tree;

   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_field %p\nline: %d\n", field, field->common.source_line);
   fprintf(fd, "|\n");

   fprintf(fd, "name: %s\n", field->name);
   if (field->ename) fprintf(fd, "ename: %s\n", field->ename);

   if (field->depending)  fprintf(fd, "depending: %p\n",  field->depending);
   if (field->false_88)   fprintf(fd, "false_88: %p\n",   field->false_88);
   if (field->index_list) fprintf(fd, "index_list: %p\n", field->index_list);

   if (field->parent) fprintf(fd, "parent: %p\n", field->parent);
   if (field->children) fprintf(fd, "children: %p\n", field->children);
   if (field->validation) fprintf(fd, "validation: %p\n", field->validation);
   if (field->sister) fprintf(fd, "sister: %p\n", field->sister);
   if (field->redefines) fprintf(fd, "redefines: %p\n", field->redefines);
   if (field->rename_thru) fprintf(fd, "rename_thru: %p\n", field->rename_thru);
   if (field->index_qual) fprintf(fd, "index_qual: %p\n", field->index_qual);
   if (field->file) fprintf(fd, "file: %p\n", field->file);
   if (field->cd) fprintf(fd, "cd: %p\n", field->cd);
   if (field->keys) fprintf(fd, "keys: %p\n", field->keys);
   if (field->pic) fprintf(fd, "PIC: %s\n", field->pic->orig);
   if (field->vsize) fprintf(fd, "vsize: %p\n", field->vsize);
   if (field->debug_section) fprintf(fd, "debug_section: %p\n", field->debug_section);

   if (field->xref.head || field->xref.tail || field->xref.skip)
   {
      fprintf(fd, "xref: %p %p %d\n", field->xref.head, field->xref.tail, field->xref.skip);
   }

   if (field->screen_line)   fprintf(fd, "screen_line: %p\n",   field->screen_line);
   if (field->screen_column) fprintf(fd, "screen_column: %p\n", field->screen_column);
   if (field->screen_from)   fprintf(fd, "screen_from: %p\n",   field->screen_from);
   if (field->screen_to)     fprintf(fd, "screen_to: %p\n",     field->screen_to);
   if (field->screen_foreg)  fprintf(fd, "screen_foreg: %p\n",  field->screen_foreg);
   if (field->screen_backg)  fprintf(fd, "screen_backg: %p\n",  field->screen_backg);
   if (field->screen_prompt) fprintf(fd, "screen_prompt: %p\n", field->screen_prompt);

   fprintf(fd, "id: %d\n", field->id);
   fprintf(fd, "size: %d\n", field->size);
   fprintf(fd, "level: %d\n", field->level);
   fprintf(fd, "memory_size: %d\n", field->memory_size);
   if (field->offset)     fprintf(fd, "offset: %d\n",     field->offset);
   if (field->occurs_min) fprintf(fd, "occurs_min: %d\n", field->occurs_min);
   if (field->occurs_max) fprintf(fd, "occurs_max: %d\n", field->occurs_max);
   if (field->indexes)    fprintf(fd, "indexes: %d\n",    field->indexes);

   if (field->count)         fprintf(fd, "count: %d\n",         field->count);
   if (field->mem_offset)    fprintf(fd, "mem_offset: %d\n",    field->mem_offset);
   if (field->nkeys)         fprintf(fd, "nkeys: %d\n",         field->nkeys);
   if (field->param_num)     fprintf(fd, "param_num: %d\n",     field->param_num);
   if (field->screen_flag)   fprintf(fd, "screen_flag: %lld\n", field->screen_flag);
   if (field->step_count)    fprintf(fd, "step_count: %d\n",    field->step_count);
   if (field->vaddr)         fprintf(fd, "vaddr: %d\n",         field->vaddr);
   if (field->odo_level)     fprintf(fd, "odo_level: %d\n",     field->odo_level);
   if (field->special_index) fprintf(fd, "special_index: %d\n", field->special_index);

   if (field->storage) fprintf(fd, "storage: %d\n", field->storage);
   if (field->usage)   fprintf(fd, "usage: %d\n",   field->usage);

   if (field->flag_base)          fprintf(fd, "flag_base: %d\n",          field->flag_base);
   if (field->flag_external)      fprintf(fd, "flag_external: %d\n",      field->flag_external);
   if (field->flag_local_storage) fprintf(fd, "flag_local_storage: %d\n", field->flag_local_storage);
   if (field->flag_is_global)     fprintf(fd, "flag_is_global: %d\n",     field->flag_is_global);

   if (field->flag_local)         fprintf(fd, "flag_local: %d\n",         field->flag_local);
   if (field->flag_occurs)        fprintf(fd, "flag_occurs: %d\n",        field->flag_occurs);
   if (field->flag_sign_clause)   fprintf(fd, "flag_sign_clause: %d\n",   field->flag_sign_clause);
   if (field->flag_sign_separate) fprintf(fd, "flag_sign_separate: %d\n", field->flag_sign_separate);
   if (field->flag_sign_leading)  fprintf(fd, "flag_sign_leading: %d\n",  field->flag_sign_leading);
   if (field->flag_blank_zero)    fprintf(fd, "flag_blank_zero: %d\n",    field->flag_blank_zero);
   if (field->flag_justified)     fprintf(fd, "flag_justified: %d\n",     field->flag_justified);
   if (field->flag_binary_swap)   fprintf(fd, "flag_binary_swap: %d\n",   field->flag_binary_swap);

   if (field->flag_real_binary)  fprintf(fd, "flag_real_binary: %d\n",  field->flag_real_binary);
   if (field->flag_is_pointer)   fprintf(fd, "flag_is_pointer: %d\n",   field->flag_is_pointer);
   if (field->flag_item_78)      fprintf(fd, "flag_item_78: %d\n",      field->flag_item_78);
   if (field->flag_any_length)   fprintf(fd, "flag_any_length: %d\n",   field->flag_any_length);
   if (field->flag_item_based)   fprintf(fd, "flag_item_based: %d\n",   field->flag_item_based);
   if (field->flag_filler)       fprintf(fd, "flag_filler: %d\n",       field->flag_filler);
   if (field->flag_synchronized) fprintf(fd, "flag_synchronized: %d\n", field->flag_synchronized);
   if (field->flag_invalid)      fprintf(fd, "flag_invalid: %d\n",      field->flag_invalid);

   if (field->flag_field)        fprintf(fd, "flag_field: %d\n",        field->flag_field);
   if (field->flag_chained)      fprintf(fd, "flag_chained: %d\n",      field->flag_chained);
   if (field->flag_anylen_done)  fprintf(fd, "flag_anylen_done: %d\n",  field->flag_anylen_done);
   if (field->flag_indexed_by)   fprintf(fd, "flag_indexed_by: %d\n",   field->flag_indexed_by);
   if (field->flag_is_verified)  fprintf(fd, "flag_is_verified: %d\n",  field->flag_is_verified);
   if (field->flag_is_c_long)    fprintf(fd, "flag_is_c_long: %d\n",    field->flag_is_c_long);
   if (field->flag_is_pdiv_parm) fprintf(fd, "flag_is_pdiv_parm: %d\n", field->flag_is_pdiv_parm);
   if (field->flag_is_pdiv_opt)  fprintf(fd, "flag_is_pdiv_opt: %d\n",  field->flag_is_pdiv_opt);

   if (field->flag_local_alloced) fprintf(fd, "flag_local_alloced: %d\n", field->flag_local_alloced);
   if (field->flag_no_init)       fprintf(fd, "flag_no_init: %d\n",       field->flag_no_init);
   if (field->flag_vsize_done)    fprintf(fd, "flag_vsize_done: %d\n",    field->flag_vsize_done);
   if (field->flag_vaddr_done)    fprintf(fd, "flag_vaddr_done: %d\n",    field->flag_vaddr_done);
   if (field->flag_odo_relative)  fprintf(fd, "flag_odo_relative: %d\n",  field->flag_odo_relative);
   if (field->flag_field_debug)   fprintf(fd, "flag_field_debug: %d\n",   field->flag_field_debug);
   if (field->flag_all_debug)     fprintf(fd, "flag_all_debug: %d\n",     field->flag_all_debug);
   if (field->flag_no_field)      fprintf(fd, "flag_no_field: %d\n",      field->flag_no_field);


   if (field->flag_any_numeric)       fprintf(fd, "flag_any_numeric: %d\n",       field->flag_any_numeric);
   if (field->flag_is_returning)      fprintf(fd, "flag_is_returning: %d\n",      field->flag_is_returning);
   if (field->flag_unbounded)         fprintf(fd, "flag_unbounded: %d\n",         field->flag_unbounded);
   if (field->flag_constant)          fprintf(fd, "flag_constant: %d\n",          field->flag_constant);
   if (field->flag_internal_constant) fprintf(fd, "flag_internal_constant: %d\n", field->flag_internal_constant);

   /* VALUES */
   if (field->values) fprintf(fd, "values_tag: %d\n", field->values->tag);
   if (field->values) fprintf(fd, "values: %p\n", field->values);

   for (tree = field->values; tree; tree = CB_CHAIN (tree)) {
      cb_tree tt = CB_VALUE (tree);
      if (tt->tag == CB_TAG_LITERAL)
      {
         rose_convert_cb_literal(fd, CB_LITERAL (tt));
      }
      else
      {
         fprintf(fd, "WARNING: value is not a literal, tag=%d\n", tt->tag);
      }
   }

   return 0;
}

int rose_convert_cb_literal(FILE* fd, struct cb_literal* lit)
{
   fprintf(fd, "--------------\n");
   fprintf(fd, "cb_literal\nline: %d\n", lit->common.source_line);
   fprintf(fd, "|\n");
   fprintf(fd, "data: %s\n", lit->data);
   fprintf(fd, "size: %d\n", lit->size);
   if (lit->scale) fprintf(fd, "scale: %d\n", lit->scale);
   if (lit->llit)  fprintf(fd, "llit: %d\n",  lit->llit);
   if (lit->sign)  fprintf(fd, "sign: %d\n",  lit->sign);
   if (lit->all)   fprintf(fd, "all: %d\n",   lit->all);
   return 0;
}

int rose_convert_cb_label(FILE* fd, struct cb_label* label)
{

   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_label\nline: %d\n", label->common.source_line);
   fprintf(fd, "|\n");

   if (label->name)
   fprintf(fd, "name: %s\n", label->orig_name);
   fprintf(fd, "id: %d\n", label->id);
   fprintf(fd, "section_id: %d\n", label->section_id);
   fprintf(fd, "segment: %d\n", label->segment);

   if (label->section)       fprintf(fd, "section: %p\n",       label->section);
   if (label->debug_section) fprintf(fd, "debug_section: %p\n", label->debug_section);
   if (label->para_label)    fprintf(fd, "para_label: %p\n",    label->para_label);
   if (label->exit_label)    fprintf(fd, "exit_label: %p\n",    label->exit_label);
   if (label->alter_gotos)   fprintf(fd, "alter_gotos: %p\n",   label->alter_gotos);
   if (label->exit_label)    fprintf(fd, "exit_label: %p\n",    label->exit_label);

   if (label->xref.head || label->xref.tail || label->xref.skip)
   {
      fprintf(fd, "xref: %p %p %d\n", label->xref.head, label->xref.tail, label->xref.skip);
   }

   if (label->flag_section)          printf("flag_section: %d\n",          label->flag_section);
   if (label->flag_entry)            printf("flag_entry: %d\n",            label->flag_entry);
   if (label->flag_begin)            printf("flag_begin: %d\n",            label->flag_begin);
   if (label->flag_return)           printf("flag_return: %d\n",           label->flag_return);
   if (label->flag_real_label)       printf("flag_real_label: %d\n",       label->flag_real_label);
   if (label->flag_global)           printf("flag_global: %d\n",           label->flag_global);
   if (label->flag_declarative_exit) printf("flag_declarative_exit: %d\n", label->flag_declarative_exit);
   if (label->flag_declaratives)     printf("flag_declaratives: %d\n",     label->flag_declaratives);
   
   if (label->flag_fatal_check)      printf("flag_fatal_check: %d\n",     label->flag_fatal_check);
   if (label->flag_dummy_section)    printf("flag_dummy_section: %d\n",   label->flag_dummy_section);
   if (label->flag_dummy_paragraph)  printf("flag_dummy_paragraph: %d\n", label->flag_dummy_paragraph);
   if (label->flag_dummy_exit)       printf("flag_dummy_exit: %d\n",      label->flag_dummy_exit);
   if (label->flag_next_sentence)    printf("flag_next_sentence: %d\n",   label->flag_next_sentence);
   if (label->flag_default_handler)  printf("flag_default_handler: %d\n", label->flag_default_handler);
   if (label->flag_statement)        printf("flag_statement: %d\n",       label->flag_statement);
   if (label->flag_first_is_goto)    printf("flag_first_is_goto: %d\n",   label->flag_first_is_goto);

   if (label->flag_alter)            printf("flag_alter: %d\n",          label->flag_alter);
   if (label->flag_debugging_mode)   printf("flag_debugging_mode: %d\n", label->flag_debugging_mode);
   if (label->flag_is_debug_sect)    printf("flag_is_debug_sect: %d\n",  label->flag_is_debug_sect);
   if (label->flag_skip_label)       printf("flag_skip_label: %d\n",     label->flag_skip_label);

   return 0;
}

int rose_convert_cb_perform(FILE* fd, struct cb_perform* perf)
{
   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_perform\nline: %d\n", perf->common.source_line);
   fprintf(fd, "|\n");
   fprintf(fd, "type: %d\n", perf->perform_type);

   return 0;
}

int rose_convert_cb_statement(FILE* fd, struct cb_statement* stmt)
{
   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_statement\nline: %d\n", stmt->common.source_line);
   fprintf(fd, "|\n");

   if (stmt->name)           printf("name: %s\n",           stmt->name);
   if (stmt->ex_handler)     printf("ex_handler: %p\n",     stmt->ex_handler);
   if (stmt->not_ex_handler) printf("not_ex_handler: %p\n", stmt->not_ex_handler);
   if (stmt->handler3)       printf("handler3: %p\n",       stmt->handler3);
   if (stmt->null_check)     printf("null_check: %p\n",     stmt->null_check);
   if (stmt->debug_check)    printf("debug_check: %p\n",    stmt->debug_check);
   if (stmt->debug_nodups)   printf("debug_nodups: %p\n",   stmt->debug_nodups);
   if (stmt->attr_ptr)       printf("attr_ptr: %p\n",       stmt->attr_ptr);

   if (stmt->handler_type)  printf("handler_type: %d\n",  stmt->handler_type);
   if (stmt->flag_no_based) printf("flag_no_based: %d\n", stmt->flag_no_based);
   if (stmt->flag_in_debug) printf("flag_in_debug: %d\n", stmt->flag_in_debug);
   if (stmt->flag_merge)    printf("flag_merge: %d\n",    stmt->flag_merge);
   if (stmt->flag_callback) printf("flag_callback: %d\n", stmt->flag_callback);

   if (stmt->body) {
      printf("body_tag: %d\n", stmt->body->tag);
      printf("body\n");
      if (stmt->body->tag == CB_TAG_LIST) {
         cb_tree list;
         for (list = stmt->body; list; list = CB_CHAIN (list)) {
            cb_tree head = CB_VALUE(list);
            /* FUNCALL */
            if (head->tag == CB_TAG_FUNCALL) {
               rose_convert_cb_funcall(fd, CB_FUNCALL(head));
            }
         }
      }
   }

   return 0;
}

int rose_convert_cb_funcall(FILE* fd, struct cb_funcall* func)
{
   int i;

   fprintf(fd, "\n--------------\n");
   fprintf(fd, "cb_funcall\n");
   fprintf(fd, "|\n");

   if (func->name)      fprintf(fd, "name: %s\n", func->name);
   if (func->argc)      fprintf(fd, "argc: %d\n", func->argc);
 //if (func->argv)      fprintf(fd, "argv: %p\n", func->argv);
   if (func->screenptr) fprintf(fd, "screenptr: %d\n", func->screenptr);
   if (func->nolitcast) fprintf(fd, "nolitcast: %d\n", func->nolitcast);
   for (i = 0; i < func->argc; i++) {
      cb_tree l;
      if (func->varcnt && i + 1 == func->argc) {
         /* output number of var_args count */
         fprintf(fd, "varcnt: %d\n", func->varcnt);
         /* output var_arg arguments */
         for (l = func->argv[i]; l; l = CB_CHAIN (l)) {
            rose_convert_cb_param(fd, CB_VALUE (l), i);
            i++;
         }
      } else {
         /* output regular function arguments before var_arg count */
         rose_convert_cb_param(fd, func->argv[i], i);
      }

   }

   return 0;
}

int rose_convert_cb_param(FILE* fd, cb_tree x, int id)
{
   fprintf(fd, "arg[%d]: ", id);

   switch (CB_TREE_TAG (x)) {
   case CB_TAG_INTEGER: {
      int ival =  CB_INTEGER (x)->val;
      fprintf(fd, "integer = %d\n", ival);
      break;
   }
   case CB_TAG_REFERENCE: {
      struct cb_reference* r = CB_REFERENCE (x);
      struct cb_field* f = CB_FIELD (r->value);
      fprintf(fd, "field = %s\n", f->name);
      break;
   }
   default:
      fprintf(fd, "rose_convert_cb_param: WARNING, tag %d not handled\n", CB_TREE_TAG(x));
   }


   return 0;
}
