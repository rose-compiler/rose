class XSValue // : public XMemory
   {
     public:

         ~XSValue();

          struct XSValue_Data 
             {
#if 0
            // Using a named scope works fine.
               union ABC
#else
            // Using an un-named scope was a bug.
               union
#endif
                  {
                    bool      f_bool;
                    double    f_double;
                  } fValue;

             } fData;
   };

XSValue::~XSValue()
   {
#if 1
  // Unparsed as:   (this) -> fData . XSValue_Data::fValue . XSValue_Data::f_double;
     fData.fValue.f_double;
#endif

#if 0
     (this) -> fData . XSValue_Data::fValue . XSValue_Data::f_double;
#endif
   }
