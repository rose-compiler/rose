#if 0
The following test program used to work in ROSE (last version I know that it worked for sure was 0.8.6a)

#include <malloc.h>
#include <stdlib.h>
void *(*Malloc)(unsigned int)= (void*(*)(unsigned int))malloc;
int  (*Free)(void *,int,char*) = (int (*)(void *, int, char *))free;


(it used to unparse to:

#include <malloc.h>
#include <stdlib.h>
void *(*Malloc)(unsigned int ) = malloc;
int (*Free)(void *, int , char *) = (int (*)(void *, int , char *))free;
)

With the newest ROSE, I get the following error:

bt: /home/winnicka/dan/sep1106/ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18878: 
     SgExpression* sage_gen_expr(an_expr_node*, int): Assertion curr_source_sequence_entry != __null failed.
Abort (core dumped)

--Beata

#endif

#include <stdlib.h>
void *(*Malloc)(unsigned int)= (void*(*)(unsigned int))malloc;
int  (*Free)(void *,int,char*) = (int (*)(void *, int, char *))free;
