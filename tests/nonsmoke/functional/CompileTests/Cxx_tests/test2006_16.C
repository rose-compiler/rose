#if 0
The following code compiled with ROSE:
char commentString[2];
void x(){
                if(*commentString!='\0');
}
gives the following error:
identityTranslator:
/home/saebjornsen1/ROSE-October/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:5863:
SgExpression* sage_form_address_constant(a_constant*, int, int,
an_il_to_str_output_control_block*, Sg_File_Info*): Assertion fileInfo->ok() failed.
#endif

char commentString[2];
void x()
   {
     if ( *commentString != '\0' );
   }

