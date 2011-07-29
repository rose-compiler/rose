class A
   {
     private:
          class xxx {};

     public:
         typedef xxx referenced_t;
   };

A::referenced_t* foobar()
   {
  // This is unparsed as: "A::xxx()" which is not visible.
  // return A::referenced_t();
     return 0L;
   }

#if 0
// Failing name qualifications:

/* Wrapper for ::SgAsmGenericStrtab::get_storage_list referenced_t ()() */
referenced_t * _haskell_stub_L37893R__L37868R(const class SgAsmGenericStrtab * v0) {
	referenced_t  rv = v0->get_storage_list();
	referenced_t *urv = new referenced_t(rv);
	return urv;
}

/* Wrapper for ::SgAsmGenericStrtab::set_storage_list void (referenced_t ) */
void  _haskell_stub_L37915R__L37916R(class SgAsmGenericStrtab * v0, referenced_t * v1) {
	v0->set_storage_list(*v1);
}

#endif
