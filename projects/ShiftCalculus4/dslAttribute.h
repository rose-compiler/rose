// Note that this header file is read by both the attributeGenerator and
// the input code to the attribute generator (both are required, I think).

// Use a default parameter to permit specification without a type. ???
template <typename T>
class DslClassAttribute 
   {
     private:
          T value;

     public:
       // We will initially be using the default constructor.
          DslClassAttribute();

       // I expect we will need a copy constructor (commented out to simplify the AST).
       // DslClassAttribute(const DslAttribute & X) {};

   };

// Alternative template arguments
// void (Bar::*BarSetterFunction)(const BazReturnType &)

// Note that type qualifier is not allowed on nonmember function (but this is a member function).
// template <class T, T T::*TemplateFunctionPointerArgument(const T &) const>
// template <class T, T T::*TemplateFunctionPointerArgument(const T &)>
// template <class T, void (T::*TemplateFunctionPointerArgument)()>
template <class T, typename returnType, returnType (T::*TemplateMemberFunctionPointerArgument)()>
class DslMemberFunctionAttribute
   {
     private:
       // TemplateMemberFunctionPointerArgument memberFunctionPointer;
       // returnType TemplateMemberFunctionPointerArgument() memberFunctionPointer;

     public:
       // We will initially be using the default constructor.
          DslMemberFunctionAttribute();

       // I expect we will need a copy constructor (commented out to simplify the AST).
       // DslMemberFunctionAttribute(const DslMemberFunctionAttribute & X) {};

   };


#if 1
template <typename returnType, returnType (*TemplateFunctionPointerArgument)()>
class DslFunctionAttribute
   {
     private:
       // T functionPointer;

     public:
       // We will initially be using the default constructor.
          DslFunctionAttribute();

       // I expect we will need a copy constructor (commented out to simplify the AST).
       // DslFunctionAttribute(const DslMemberFunctionAttribute & X) {};

   };
#endif
