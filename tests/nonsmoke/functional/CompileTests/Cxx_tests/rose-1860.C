
struct __nonesuch;

template<bool, typename, typename>
struct conditional;

template<typename T>
struct pair {
    pair & operator=(
      typename conditional<
        true,
        const pair &,
        const __nonesuch &
      >::type __p
    );

    pair & operator=(
      typename conditional<
        false,
        const pair &,
        const __nonesuch&
      >::type __p
    ) = delete;
};

