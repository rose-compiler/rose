
namespace std 
{
  template<typename _CharT, typename _Traits>
    streamsize
    __copy_streambufs_eof(basic_streambuf<_CharT, _Traits>*,
			  basic_streambuf<_CharT, _Traits>*, bool&);

  template<typename _CharT, typename _Traits>
    class basic_streambuf 
    {
    public:
#if 0
      //@{
      /**
       *  These are standard types.  They permit a standardized way of
       *  referring to names of (or names dependent on) the template
       *  parameters, which are specific to the implementation.
      */
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;
      //@}

      //@{
      /// This is a non-standard type.
      typedef basic_streambuf<char_type, traits_type>  	__streambuf_type;
      //@}
      
      friend class basic_ios<char_type, traits_type>;
      friend class basic_istream<char_type, traits_type>;
      friend class basic_ostream<char_type, traits_type>;
      friend class istreambuf_iterator<char_type, traits_type>;
      friend class ostreambuf_iterator<char_type, traits_type>;
#endif

      friend streamsize
      __copy_streambufs_eof<>(basic_streambuf*, basic_streambuf*, bool&);

  };

}
