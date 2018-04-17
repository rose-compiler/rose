
#define TEST_NOTHING                   0x000000000
#define TEST_ALL                       0xfffffffff
#define TEST_CASTS                     0x000000001
#define TEST_CASTS2                    0x000000002
#define TEST (TEST_ALL)
#define CHECK_WRAP(x, block) if (TEST & (x)) { block }


struct HasFloat;

struct HasInt
{
	int d;
//	static int num_objs;
//	HasInt() { ++num_objs; }
	virtual void foo() {};
	operator HasFloat();
	operator HasFloat*();
};
//int HasInt::num_objs = 0;
struct HasFloat
{
	float f;
};
struct HasDouble : HasInt
{
	float f;
};

HasInt::operator HasFloat()
{
	HasFloat a;
	return a;
}

HasInt::operator HasFloat*()
{
	HasFloat a;
	return &a;
}


void HasLValues()
{
	// Casts
#if TEST & TEST_CASTS
	{
		int d;
		float f;
		double g;
		char c;
//		const_cast<const int&>(4);
//		dynamic_cast<const float&>(4);
//		static_cast<const double&>(4);
//		reinterpret_cast<const char&>(4);
		(const int&)(4);
		const_cast<int&>(d);
//		dynamic_cast<float&>(f);
		static_cast<double&>(g);
		reinterpret_cast<char&>(c);
		(int&)("");
		HasInt hs;
		const_cast<HasInt&>(hs);
		dynamic_cast<HasDouble&>(hs);
		static_cast<HasInt&>(hs);
		reinterpret_cast<HasFloat&>(hs);
		(HasFloat&)(hs);
	}
#endif
}

void NoLValues()
{
	// Casts
#if TEST & TEST_CASTS
	{
//		const_cast<int>(4);
//		dynamic_cast<float>(4);
		static_cast<double>(4);
//		reinterpret_cast<char>(4);
		(int)(5.4);
		HasInt L_VALUE_hs;
		const_cast<HasInt*>(&L_VALUE_hs);
//		dynamic_cast<HasDouble*>(&L_VALUE_hs);
		static_cast<HasInt>(L_VALUE_hs);
		reinterpret_cast<HasFloat*>(&L_VALUE_hs);
		(HasFloat)(L_VALUE_hs);
	}
#endif
}

int main()
{
	return 1;
}

