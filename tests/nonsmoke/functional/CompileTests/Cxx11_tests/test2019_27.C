void foobar()
   {
     enum class color { R, O, Y, G, B, I, V };
     int c = static_cast<int>(color::R);
     c = static_cast<int>(color::G);
   }
