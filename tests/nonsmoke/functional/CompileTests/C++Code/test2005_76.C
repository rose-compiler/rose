template <class T>
class A {
public:
	void a();
	void b() { int c = 2 * 4; }
};

template <class T>
void A<T>::a() {
	int c = 2 * 3;
}

class Z {
};

int main(int ac, char *av[]) {
	A<Z> a;
	a.a();
	a.b();
}
