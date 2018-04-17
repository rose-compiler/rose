template <typename LOOP_BODY>
void for_all(LOOP_BODY loop_body)
   {
     for (int i = 0; i < 10; i++)
       {
         loop_body(i);
       }
   }

