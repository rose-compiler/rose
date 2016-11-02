
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
struct HasFloat : HasInt
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
	return a;
}


int main()
{
	HasInt hasi;
	(HasFloat)(hasi);
	return 1;
}

