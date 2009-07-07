template<class Elem = char, class Tr = std::char_traits<Elem> >
class StreamRedirector: public std::basic_streambuf<Elem, Tr>
{
    public:
        /**
         * Constructor.
         * @param a_Stream the stream to redirect
         */
        StreamRedirector(std::ostream& a_Stream)
            : m_Stream(a_Stream)
        {
            //redirect stream
            m_pBuf = m_Stream.rdbuf(this);
        };

        /**
         * Destructor.
         * Restores the original stream.
         */
        ~StreamRedirector()
        {
            m_Stream.rdbuf(m_pBuf);
        }

        /**
         * Override xsputn and make it forward data to the callback function.
         */
        std::streamsize xsputn(const Elem* _Ptr, std::streamsize _Count)
        {
            m_pCbFunc(_Ptr, _Count, m_pUserData);
            return _Count;
        }

        /**
         * Override overflow and make it forward data to the callback function.
         */
        typename Tr::int_type overflow(typename Tr::int_type v)
        {
            Elem ch = Tr::to_char_type(v);
            m_pCbFunc(&ch, 1, m_pUserData);
            return Tr::not_eof(v);
        }

    protected:
        std::string value;

        std::basic_ostream<Elem, Tr>& m_Stream;
        std::streambuf* m_pBuf;
        pfncb m_pCbFunc;
        void* m_pUserData;
};
