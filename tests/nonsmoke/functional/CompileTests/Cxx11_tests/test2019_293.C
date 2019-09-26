namespace M {
	struct X {
		int i;
		X() : i(0) { }
	};
}
namespace N {
	struct X {
		int i;
		X() : i(0) { }
		int h(M::X) { return 3; }
	};
}

int (N::X::*pmf_)(M::X) = &N::X::h;

namespace N {
	int f_(int (N::X::*)(M::X)) { return 12; }
}
namespace M {
	int g_(int (N::X::*)(M::X)) { return 1013; }
}


void foobar()
   {
     int n = f_(pmf_);
     int m = g_(pmf_);
}

