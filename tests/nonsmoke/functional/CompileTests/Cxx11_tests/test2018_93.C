enum class EnumClass1 {
   enumValue1 = 1
};

int func2( EnumClass1 parm1 ) {
   switch(parm1)
   {
   // generated code should be: 
   // case EnumClass1::enumValue1: 
      case EnumClass1::enumValue1: return 0; 
   }
}

EnumClass1 func1() {
  return EnumClass1::enumValue1;
}

