/*
when compiling the following code in ROSE:

typedef void ( *PLDestroyEventProc)();

void PL_InitEvent(PLDestroyEventProc destructor);

class nsGnomeVFSSetContentTypeEvent
{
 public:
   nsGnomeVFSSetContentTypeEvent()
   {
     PL_InitEvent(EventDestructor);
   }

   static void EventDestructor()
   {
   }
};

I get the following error:

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:892:
SgExpression* sage_form_symbol(a_source_correspondence*, a_constant*):
Assertion sym != __null failed. 
*/

typedef void ( *PLDestroyEventProc)();

void PL_InitEvent(PLDestroyEventProc destructor);

class nsGnomeVFSSetContentTypeEvent
{
 public:
   nsGnomeVFSSetContentTypeEvent()
   {
     PL_InitEvent(EventDestructor);
   }

   static void EventDestructor()
   {
   }
};

