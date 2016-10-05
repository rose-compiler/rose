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
  // DQ (8/14/2006): This was allowed by g++ but not by EDG, it was fixed directly 
  // in the EDG source code when we worked on using the Intel compiler with ROSE 
  // (compiling both EDG source and ROSE source using the Intel icpc compiler).
  // As a result this is a little less critical to fix, plus there is little to do 
  // about it since the issue is with EDG, and I think that EDG is correct in this 
  // case and g++ is being lax on the subject of implicit casting of const.

  // AS and PC (060206) changed to cast away const (example where this is fixed in EDG.
  // write_tok_str(false ? "__asm(" : "asm(");
     write_tok_str(false ? (char*)"__asm(" : (char*)"asm(");
   }

