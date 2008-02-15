
int main(int argc, char *argv[]) {
	double i;

	{
		int i = 0;
		{
			i = 2;
		}
		i = i + 1;
	}

	{ 
		char i = 'a';
		i++;
	}

	i = 1.0;
	return i;

}


