
 
namespace std
{
  
  template<typename _Tp> class complex;
  template<> class complex<float>;
  template<> class complex<double>;
  template<> class complex<long double>;


  template<typename _Tp>
    struct complex
    {
      
      typedef _Tp value_type;
      
      
      
       complex(const _Tp& __r = _Tp(), const _Tp& __i = _Tp())
      : _M_real(__r), _M_imag(__i) { }

      
      
      
      template<typename _Up>
         complex(const complex<_Up>& __z)
	: _M_real(__z.real()), _M_imag(__z.imag()) { }

      
      _Tp& 
      real() { return _M_real; }

      
      const _Tp& 
      real() const { return _M_real; }

      
      _Tp& 
      imag() { return _M_imag; }

      
      const _Tp& 
      imag() const { return _M_imag; }

      
      
      void 
      real(_Tp __val) { _M_real = __val; }

      void 
      imag(_Tp __val) { _M_imag = __val; }

      
      complex<_Tp>& operator=(const _Tp&);
      
      
      
      complex<_Tp>&
      operator+=(const _Tp& __t)
      {
	_M_real += __t;
	return *this;
      }

      
      
      complex<_Tp>&
      operator-=(const _Tp& __t)
      {
	_M_real -= __t;
	return *this;
      }

      
      complex<_Tp>& operator*=(const _Tp&);
      
      complex<_Tp>& operator/=(const _Tp&);

      
      
      
      
      template<typename _Up>
        complex<_Tp>& operator=(const complex<_Up>&);
      
      template<typename _Up>
        complex<_Tp>& operator+=(const complex<_Up>&);
      
      template<typename _Up>
        complex<_Tp>& operator-=(const complex<_Up>&);
      
      template<typename _Up>
        complex<_Tp>& operator*=(const complex<_Up>&);
      
      template<typename _Up>
        complex<_Tp>& operator/=(const complex<_Up>&);

      const complex __rep() const
      { return *this; }

    private:
      _Tp _M_real;
      _Tp _M_imag;
    };


  template<>
    struct complex<float>
    {
      typedef float value_type;
      typedef __complex__ float _ComplexT;

       complex(_ComplexT __z) : _M_value(__z) { }

      complex&
      operator=(float __f)
      {
	_M_value = __f;
	return *this;
      }

      template<typename _Tp>
        complex&
        operator=(const complex<_Tp>&  __z)
	{
	  __real__ _M_value = __z.real();
	  __imag__ _M_value = __z.imag();
	  return *this;
	}

    private:
      _ComplexT _M_value;
    };



  template<>
    struct complex<double>
    {
      typedef double value_type;
      typedef __complex__ double _ComplexT;

       complex(_ComplexT __z) : _M_value(__z) { }

       complex(double __r = 0.0, double __i = 0.0)
      {
	__real__ _M_value = __r;
	__imag__ _M_value = __i;
      }

       complex(const complex<float>& __z)
       //   : _M_value(__z.__rep())
        { }

    private:
      _ComplexT _M_value;
    };

}

namespace std
{

    template<>
    inline complex<float>&
        complex<float>::operator=(const complex<double>& __z)
    {
        _M_value = __z._M_value;
        return *this;
    }

}

