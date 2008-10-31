//Test code using pointer comparison
typedef struct gun{
   char name [256];
   int magzing_size;
   float calibre;
} gun_t; 

int main()
{
  gun_t gun1,gun2;
  gun_t *gun1_p, *gun2_p;

  gun1_p = & gun1;
  gun2_p = & gun2;

  if (gun1_p<gun2_p)
  {
    return 0;
  }
  else
    return 1;
}

