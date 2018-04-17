
// Test the scope of the labels.
// They should be put into the function scope, or at least the scope of the body of the function.
// But currently they are put into the scope where they are declared.  Both the SgLabelSymbol and
// the scope of the SgLabelStatement need to be fixed up to be correct and match (be consistant
// so that get_symbol_from_symbol_table () works properly).

void foo()
   {
     int x = 7;
// my_label: 
     goto my_nested_label;
     if (x >0)
        {
my_nested_label:
          x=2;
      }
     goto my_nested_label;
 // goto my_label;
   }
