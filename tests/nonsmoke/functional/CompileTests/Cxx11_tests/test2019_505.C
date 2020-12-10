// ROSE-2124

template<typename _Functor>
class _Base_manager
   {
     public:
          static void _M_init_functor() {} 

       // NEEDED:
          template<typename _Class, typename _Tp>
          static bool _M_not_empty_function(_Tp _Class::* const& __mp) { return true; }
   }; 

template<typename _Signature, typename _Functor> class _Function_handler;

template<typename _Res, typename _Functor, typename... _ArgTypes>
class _Function_handler<_Res(_ArgTypes...), _Functor>
   : public _Base_manager<_Functor> 
   {
   }; 

template<typename _Signature> class function;

template<typename _Res, typename... _ArgTypes>
class function<_Res(_ArgTypes...)>
   {
          typedef _Res _Signature_type(_ArgTypes...);

     public:
          template<typename _Functor, typename = void> function(_Functor);
   }; 

template<typename _Res, typename... _ArgTypes>
template<typename _Functor, typename> function<_Res(_ArgTypes...)>::function(_Functor __f)
   {
  // NEEDED:
     _Function_handler<_Signature_type, _Functor>::_M_init_functor();
   }

class Class_3 
   {
     public:
          void func_4();
   };

template<typename t_t_parm_1>
class Class_2 
   {
     public:
          template<typename t_t_parm_2>
          Class_2(t_t_parm_2 &parm_4);

          t_t_parm_1 *operator->();
   }; 

void func_3(int &parm_3)
   {
     Class_2<Class_3> local_1(parm_3);
     function<void()> func_1 = [&]() { };
     local_1->func_4();
   }

#if 0
gets the error
identityTranslator: Cxx_Grammar.C:69689: static SgModifierType *SgModifierType::insertModifierTypeIntoTypeTable(SgModifierType *): Assertion t->stripType() == result->stripType() failed.
#endif

