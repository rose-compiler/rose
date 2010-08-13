struct Foo
{
	int i;
	mutable int mi;
};

const Foo foo = {1};

int main()
{
	foo.i;
	foo.mi;
}

