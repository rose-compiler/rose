int main(int argc,char**argv)
{
	int count=50;
	int sum=0;
	int term=10;
	switch (count % 8)  /* count > 0 assumed */
  {
		case 0:        do {  sum += term;
		case 7:              sum += term;
		case 6:              sum += term;
		case 5:              sum += term;
		case 4:              sum += term;
		case 3:              sum += term;
		case 2:              sum += term;
		case 1:              sum += term;
		} while ((count -= 8) > 0);
	}
	return 0;
}
	
