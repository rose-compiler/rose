// This causes an error (and might be a bug):
// preprocessor: /home/dquinlan/ROSE/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:752: SgName sage_gen_unqualified_name(a_source_correspondence*, an_il_entry_kind): Assertion `templateDeclaration != __null' failed.

namespace std
{
  template<typename T>
    class basic_filebuf 
    {
    public:
      typedef basic_filebuf<int> __filebuf_type;

      int _M_underflow_common();

    };

  // Explicit specialization declarations, defined in src/fstream.cc.
  template<> int basic_filebuf<char>::_M_underflow_common();

} // namespace std

