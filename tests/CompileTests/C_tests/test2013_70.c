struct sbg_script_synth
   {
      int x;
   };

void foo()
   {
     struct sbg_script_synth *s1, s1mod;

  // s1mod = s1 != &s1mod ? *s1 : (struct sbg_script_synth){ 0 };
     s1mod = (s1 != &s1mod) ? *s1 : (struct sbg_script_synth){ 0 };
   }
