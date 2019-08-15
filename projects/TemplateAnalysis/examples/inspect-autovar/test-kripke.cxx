
namespace std {
  template< class T >
  constexpr T&& forward( T& t ) noexcept {};

  template< class T >
  constexpr T&& forward( T&& t ) noexcept {};
}

namespace Kripke {
  class SdomId {};
  namespace Core {
    class DataStore {};
  }

  // Layout

  struct LayoutT_1 {};
  struct LayoutT_2 {};

  enum LayoutV {
    LayoutV_1,
    LayoutV_2
  };

  template<typename Function, typename ... Args>
  void dispatchLayout(LayoutV layout_v, Function const &fcn, Args &&... args) {
    switch(layout_v){
      case LayoutV_1: fcn(LayoutT_1{}, std::forward<Args>(args)...); break;
      case LayoutV_2: fcn(LayoutT_2{}, std::forward<Args>(args)...); break;
    }
  }

  // Arch

  struct ArchT_1 {};
  struct ArchT_2 {};

  enum ArchV {
    ArchV_1,
    ArchV_2
  };

  template<typename Function, typename ... Args>
  void dispatchArch(ArchV arch_v, Function const &fcn, Args &&... args) {
    switch(arch_v){
      case ArchV_1: fcn(ArchT_1{}, std::forward<Args>(args)...); break;
      case ArchV_2: fcn(ArchT_2{}, std::forward<Args>(args)...); break;
    }
  }

  // Dispatch

  template<typename ARCH, typename LAYOUT>
  struct ArchLayoutT {
    using arch_t = ARCH;
    using layout_t = LAYOUT;
  };

  struct ArchLayoutV {
    ArchV arch_v;
    LayoutV layout_v;
  };

  class ArchLayout {
    public:
      ArchLayout() = default;
      virtual ~ArchLayout() = default;

      ArchLayoutV al_v;
  };

  template<typename arch_t>
  struct DispatchHelper {
    template<typename layout_t, typename Function, typename ... Args>
    void operator()(layout_t, Function const &fcn, Args &&... args) const {
      using al_t = ArchLayoutT<arch_t, layout_t>;
      fcn(al_t{}, std::forward<Args>(args)...);
    }
  };

  template<typename Function, typename ... Args>
  void dispatch(ArchLayoutV al_v, Function const &fcn, Args &&... args) {
    dispatchArch(al_v.arch_v, [&](auto arch_t){
      DispatchHelper<decltype(arch_t)> helper;
      dispatchLayout(al_v.layout_v, helper, fcn, std::forward<Args>(args)...);
    });
  }
}

using namespace Kripke;
using namespace Kripke::Core;

struct SweepSdom {
  template<typename AL>
  void operator() (AL al, Kripke::Core::DataStore & data_store, Kripke::SdomId sdom_id) const {
    int var;
  }
};

void foo(Kripke::Core::DataStore & data_store, Kripke::SdomId sdom_id) {
  ArchLayoutV al_1{ArchV_1,LayoutV_2};
  Kripke::dispatch(al_1, SweepSdom{}, data_store, sdom_id);
}

void bar(Kripke::Core::DataStore & data_store, Kripke::SdomId sdom_id) {
  ArchLayoutV al_2{ArchV_2,LayoutV_1};
  Kripke::dispatch(al_2, SweepSdom{}, data_store, sdom_id);
}

