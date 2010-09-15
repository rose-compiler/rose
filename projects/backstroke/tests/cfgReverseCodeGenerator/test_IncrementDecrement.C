struct State
{
  int x;
  int y;
  int count;
};

void reverseMe(int in, State* state)
{
	in = state->count * state->count + 3;
	
	state->count--;

	in = 3;
}
