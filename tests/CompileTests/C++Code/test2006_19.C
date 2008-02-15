#if 0
Compiling the following code with ROSE:
static void write_tok_str(char *str)
{
}
static void gen_asm_decl(void)
{
  write_tok_str(false ? "__asm(" : "asm(");
}

gives the following error:
"../../../../ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C", line 6:
error:
          argument of type "const char *" is incompatible with parameter of
          type "char *"
    write_tok_str(false ? "__asm(" : "asm(");

Andreas
#endif


static void write_tok_str(char *str)
   {
   }

static void gen_asm_decl(void)
   {
     write_tok_str(false ? "__asm(" : "asm(");
   }

