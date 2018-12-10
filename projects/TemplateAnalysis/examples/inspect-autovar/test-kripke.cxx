
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

  struct ArchT_Sequential {};

  struct LayoutT_DGZ {};

  template<typename ARCH, typename LAYOUT>
  struct ArchLayoutT {
    using arch_t = ARCH;
    using layout_t = LAYOUT;
  };

  class ArchLayout {
    public:
      ArchLayout() = default;
      virtual ~ArchLayout() = default;
  };

  template<typename Function, typename ... Args>
  void dispatchLayout(Function const &fcn, Args &&... args) {
    fcn(LayoutT_DGZ{}, std::forward<Args>(args)...);
  }

  template<typename Function, typename ... Args>
  void dispatchArch(Function const &fcn, Args &&... args) {
    fcn(ArchT_Sequential{}, std::forward<Args>(args)...);
  }

  template<typename arch_t>
  struct DispatchHelper {
    template<typename layout_t, typename Function, typename ... Args>
    void operator()(layout_t, Function const &fcn, Args &&... args) const {
      using al_t = ArchLayoutT<arch_t, layout_t>;
      fcn(al_t{}, std::forward<Args>(args)...);
    }
  };

  template<typename Function, typename ... Args>
  void dispatch(Function const &fcn, Args &&... args) {
    dispatchArch([&](auto arch_t){
      DispatchHelper<decltype(arch_t)> helper;
      dispatchLayout(helper, fcn, std::forward<Args>(args)...);
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
  Kripke::dispatch(SweepSdom{}, data_store, sdom_id);
}

