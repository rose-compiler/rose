// This code appears in flex_string.hpp as part of boost.
// It is used in ROSE, so ROSE will not compile ROSE until
// we can handle this construction of switch statements.

// This is a variation of Duff's device.

// NOTE: that because this is in a template and we are unparsing 
// the template as a string, we get this case perfect.  However,
// it is not correctly represented in the AST, and a non template
// example is required to see this (in the unparsed code).

template <class Pod, class T>
inline void pod_fill(Pod* b, Pod* e, T c)
   {
     switch ((e - b) & 7)
        {
          case 0:
               while (b != e)
                  {
                    *b = c; ++b;
            case 7: *b = c; ++b;
            case 6: *b = c; ++b;
            case 5: *b = c; ++b;
            case 4: *b = c; ++b;
            case 3: *b = c; ++b;
            case 2: *b = c; ++b;
            case 1: *b = c; ++b;
                  }
        }
   }

