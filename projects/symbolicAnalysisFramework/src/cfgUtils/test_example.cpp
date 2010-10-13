void start_save(){}

int main()
{
	return 0;
}

class myClass
{
	public:
	short x;
	int y;
	long int z;
};

typedef struct 
{
	int a;
	int b;
	myClass theClass;
} myStruct;


int testExprParsing_0(int arg)
{
	return arg;
}

int testExprParsing_1()
{
	int i=1, j=2;
	myStruct q;
	
	q.b=1;
	q.a=q.b+(char)5;
	i = j+1;
	q.theClass.y = i;
	j = q.theClass.z;
	q.theClass.z = -98;
	q.a=q.b-5;
	i = j-(long)12;
	q.b += 2;
	q.b += q.a;
	q.b -= q.a;
	q.theClass.x -= (unsigned int)16;
	q.b += -32;
	q.theClass.x -= (short)-33;
	q.theClass.x++;
	q.theClass.x--;
	++q.theClass.x;
	--q.theClass.x;
	q.a = q.b + j;
	q.a = q.b - j;
	q.a = 5 - j;
	i *= 5;
	i /= j;
	i = j / -5;
	i = j * -5;
	q.b = q.theClass.x * i;
	q.b = q.theClass.x / i;
	
	if(i-5          <= j+1){}
	if(q.a          <= j+1){}
	if(i            <= q.a-1){}
	if(q.theClass.z <= 1){}
	if(i            <= -1){}
	if(1            <= q.a){}
	if(-1           <= j){}
	if(-761         <= j+220){}
	if(i+761        <= q.theClass.z){}

	if(i-5          >= j+1){}
	if(q.a          >= j+1){}
	if(i            >= q.a-1){}
	if(q.theClass.z >= 1){}
	if(i            >= -1){}
	if(1            >= q.a){}
	if(-1           >= j){}
	if(-761         >= j+220){}
	if(i-761        >= q.theClass.z){}
		
	if(i-5          < j+1){}
	if(q.a          < j+1){}
	if(i            < q.a-1){}
	if(q.theClass.z < 1){}
	if(i            < -1){}
	if(1            < q.a){}
	if(-1           < j){}
	if(-761         < j+220){}
	if(i-761        < q.theClass.z){}
		
	if(i-5          > j+1){}
	if(q.a          > j+1){}
	if(i            > q.a-1){}
	if(q.theClass.z > 1){}
	if(i            > -1){}
	if(1            > q.a){}
	if(-1           > j){}
	if(-761         > j+220){}
	if(i-761        > q.theClass.z){}
		
	if(i-5          == j+1){}
	if(q.a          == j+1){}
	if(i            == q.a-1){}
	if(q.theClass.z == 1){}
	if(i            == -1){}
	if(1            == q.a){}
	if(-1           == j){}
	if(-761         == j+220){}
	if(i+761        == q.theClass.z){}

	if(i-5          == j+1){}
	if(q.a          == j+1){}
	if(i            == q.a-1){}
	if(q.theClass.z == 1){}
	if(i            == -1){}
	if(1            == q.a){}
	if(-1           == j){}
	if(-761         == j+220){}
	if(i-761        == q.theClass.z){}
		
	i *= j + 6;
	i = j / 5 + 1;
	2+testExprParsing_0(i*9);
	if(i*1 < j){}
	if(i==j){}
	if(q.a = (i<=j)){}
	
	return i;
}

int temp_, temp_0, temp_1, temp_2, temp_3, temp_4, temp_5, temp_6, temp_7, temp_8, temp_9;
int temp_00, temp_10, temp_20, temp_30, temp_40, temp_50, temp_60, temp_70, temp_80, temp_90;
int temp_01, temp_11, temp_21, temp_31, temp_41, temp_51, temp_61, temp_71, temp_81, temp_91;
int temp_02, temp_12, temp_22, temp_32, temp_42, temp_52, temp_62, temp_72, temp_82, temp_92;
int temp_03, temp_13, temp_23, temp_33, temp_43, temp_53, temp_63, temp_73, temp_83, temp_93;
int temp_04, temp_14, temp_24, temp_34, temp_44, temp_54, temp_64, temp_74, temp_84, temp_94;
int temp_05, temp_15, temp_25, temp_35, temp_45, temp_55, temp_65, temp_75, temp_85, temp_95;
int temp_06, temp_16, temp_26, temp_36, temp_46, temp_56, temp_66, temp_76, temp_86, temp_96;
int temp_07, temp_17, temp_27, temp_37, temp_47, temp_57, temp_67, temp_77, temp_87, temp_97;
int temp_08, temp_18, temp_28, temp_38, temp_48, temp_58, temp_68, temp_78, temp_88, temp_98;
int temp_09, temp_19, temp_29, temp_39, temp_49, temp_59, temp_69, temp_79, temp_89, temp_99;
int foo(){return 0;}
int testCFGTransform_0()
{
	int a=0,b=1,c=0;
	
	a = a = (b+c);
	
	b=c;
	
	if(a = (a=b))
	{
	}
	
	a = foo();
	
	return b+a;
}

