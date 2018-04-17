class myClass
{
   public:
     union
     {
       struct
       {
         float x;
         float y;
         float z;
       };
       float m_data[3];
     };
   protected:
};


int main(int argc, char **)

{
   myClass foo;
   foo.x=1.0;
   return 0;
}
