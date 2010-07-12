/*struct Point
{
	int x;
	int y;
};

struct Line
{
	Point p1;
	Point p2;
};*/


int main()
{
	int i;
	int j;
	int k;

	i = 17;
	j = i * 3;

	//Swap i and j and do some other stuff
	i++;
	int t = i;
	k = j * 12;
	k++;

	i = j;
	j = t;

	--i;

	//Bug in def-use. Bleh
	(t = i)++;

	//Do it on one line
	(t = i, i = k, k = t);

	

	//Some class modification
	/*Line line;
	line.p1.x = 3;
	line.p2.x = line.p1.x;
	line.p1.x++;
	line.p2.x = line.p1.x * 2;*/

	return 0;
}
