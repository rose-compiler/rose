
template<typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body)
   {
   }


template<typename TI>
void foobar_template(TI const &is_i)
   {
     int is; 
     forallN(is, [=] (int i) 
        { 
       // This CPP directive (and this comment) will be unparsed before the template declaration.
       // This is a consequence of the template unparsing from internal strings, use the template 
       // unparsing of the template declaration to be a more precise unparsing.
#if 1
          42;
#endif
        });
   }


void foobar()
   {
     int is; 
     foobar_template(is);
   }
