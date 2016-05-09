
      template<typename _Tp, typename... _Args>
	struct __construct_helper
	{
	};

      template<typename _Tp, typename... _Args>
	using __has_construct
	  = typename __construct_helper<_Tp, _Args...>::type;



__construct_helper<int,int> xxx;
