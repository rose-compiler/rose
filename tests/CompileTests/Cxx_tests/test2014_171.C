template<typename _Tp1>
struct auto_ptr_ref
   {
     explicit auto_ptr_ref(_Tp1* __p) { }
   };

template<typename _Tp>
class auto_ptr
   {
     public:
          typedef _Tp element_type;

       // explicit auto_ptr(element_type* __p = 0) throw() { }
          explicit auto_ptr(element_type* __p = 0) { }

       // Commenting this out allows the code to compile with ROSE.
       // auto_ptr(auto_ptr& __a) throw() { }
          auto_ptr(auto_ptr& __a) { }

       // auto_ptr(auto_ptr_ref<element_type> __ref) throw() { }
          auto_ptr(auto_ptr_ref<element_type> __ref) { }

       // template<typename _Tp1> operator auto_ptr_ref<_Tp1>() throw() { return auto_ptr_ref<_Tp1>(0L); }
          template<typename _Tp1> operator auto_ptr_ref<_Tp1>() { return auto_ptr_ref<_Tp1>(0L); }
   };

class MatOpCommand {};

class CleanMatOpCommand : public MatOpCommand 
   {
     public:
          CleanMatOpCommand(int min_volume_fraction) {}
   };

auto_ptr<MatOpCommand> parseCleanCommand()
   {
     double min_volume_fraction;
  // Bug: this unparses to be:
  // return ((auto_ptr< MatOpCommand > ::auto_ptr(0L) . operator auto_ptr_ref<MatOpCommand>()));
  // and should be:
  // return auto_ptr<MatOpCommand>(0L);

  // float a = float(4);

  // return auto_ptr<MatOpCommand>(new CleanMatOpCommand(42));
  // return auto_ptr<MatOpCommand>(new CleanMatOpCommand(42)).operator auto_ptr_ref<MatOpCommand>();
  // return ((auto_ptr< MatOpCommand > ::auto_ptr((new CleanMatOpCommand (42))) . operator auto_ptr_ref<MatOpCommand>()));

  // Failing code in g++ and EDG
  // return auto_ptr< MatOpCommand > ::auto_ptr(new CleanMatOpCommand (42)) . operator auto_ptr_ref<MatOpCommand>();

  // This fails for GNU g++ version 4.4.7, but works fine for EDG. So we need to detect this as a bug in GNU
  // and unparse this in operator form and supress the operator because it is implicit.
  // return auto_ptr< MatOpCommand >(new CleanMatOpCommand (42)) . operator auto_ptr_ref<MatOpCommand>();

#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 10) )
  // This is the best example of why the operator form of unparsing is not equivalent to the non-operator form.
  // This form is also a bug for the GNU 4.4.7 compiler.
     return auto_ptr< MatOpCommand >(0L) . operator auto_ptr_ref<MatOpCommand>();
#else
     return auto_ptr<MatOpCommand>(0L);
#endif
   }
