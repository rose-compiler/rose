
  template<typename _RealType = double>
    class uniform_real_distribution
    {
 // static_assert(std::is_floating_point<_RealType>::value,"template argument not a floating point type");

    public:
      /** The type of the range of the distribution. */
      typedef _RealType result_type;
      /** Parameter type. */
      struct param_type
      {
	typedef uniform_real_distribution<_RealType> distribution_type;

	explicit
	param_type(_RealType __a = _RealType(0),
		   _RealType __b = _RealType(1))
	: _M_a(__a), _M_b(__b)
	{
	  _GLIBCXX_DEBUG_ASSERT(_M_a <= _M_b);
	}

	result_type
	a() const
	{ return _M_a; }

	result_type
	b() const
	{ return _M_b; }

	friend bool
	operator==(const param_type& __p1, const param_type& __p2)
	{ return __p1._M_a == __p2._M_a && __p1._M_b == __p2._M_b; }

      private:
	_RealType _M_a;
	_RealType _M_b;
      };

    public:
      /**
       * @brief Constructs a uniform_real_distribution object.
       *
       * @param __a [IN]  The lower bound of the distribution.
       * @param __b [IN]  The upper bound of the distribution.
       */
      explicit
      uniform_real_distribution(_RealType __a = _RealType(0),
				_RealType __b = _RealType(1))
      : _M_param(__a, __b)
      { }

      explicit
      uniform_real_distribution(const param_type& __p)
      : _M_param(__p)
      { }

      /**
       * @brief Resets the distribution state.
       *
       * Does nothing for the uniform real distribution.
       */
      void
      reset() { }

      result_type
      a() const
      { return _M_param.a(); }

      result_type
      b() const
      { return _M_param.b(); }

      /**
       * @brief Returns the parameter set of the distribution.
       */
      param_type
      param() const
      { return _M_param; }

      /**
       * @brief Sets the parameter set of the distribution.
       * @param __param The new parameter set of the distribution.
       */
      void
      param(const param_type& __param)
      { _M_param = __param; }

      /**
       * @brief Returns the inclusive lower bound of the distribution range.
       */
      result_type
      min() const
      { return this->a(); }

      /**
       * @brief Returns the inclusive upper bound of the distribution range.
       */
      result_type
      max() const
      { return this->b(); }

      /**
       * @brief Generating functions.
       */
      template<typename _UniformRandomNumberGenerator>
	result_type
	operator()(_UniformRandomNumberGenerator& __urng)
        { return this->operator()(__urng, _M_param); }

#if 0
      template<typename _UniformRandomNumberGenerator>
	result_type
	operator()(_UniformRandomNumberGenerator& __urng,
		   const param_type& __p)
	{
	  __detail::_Adaptor<_UniformRandomNumberGenerator, result_type>
	    __aurng(__urng);
	  return (__aurng() * (__p.b() - __p.a())) + __p.a();
	}
#endif

      template<typename _ForwardIterator,
	       typename _UniformRandomNumberGenerator>
	void
	__generate(_ForwardIterator __f, _ForwardIterator __t,
		   _UniformRandomNumberGenerator& __urng)
	{ this->__generate(__f, __t, __urng, _M_param); }

      template<typename _ForwardIterator,
	       typename _UniformRandomNumberGenerator>
	void
	__generate(_ForwardIterator __f, _ForwardIterator __t,
		   _UniformRandomNumberGenerator& __urng,
		   const param_type& __p)
	{ this->__generate_impl(__f, __t, __urng, __p); }

      template<typename _UniformRandomNumberGenerator>
	void
	__generate(result_type* __f, result_type* __t,
		   _UniformRandomNumberGenerator& __urng,
		   const param_type& __p)
	{ this->__generate_impl(__f, __t, __urng, __p); }

      /**
       * @brief Return true if two uniform real distributions have
       *        the same parameters.
       */
      friend bool
      operator==(const uniform_real_distribution& __d1,
		 const uniform_real_distribution& __d2)
      { return __d1._M_param == __d2._M_param; }

    private:
      template<typename _ForwardIterator,
	       typename _UniformRandomNumberGenerator>
	void
	__generate_impl(_ForwardIterator __f, _ForwardIterator __t,
			_UniformRandomNumberGenerator& __urng,
			const param_type& __p);

      param_type _M_param;
    };
