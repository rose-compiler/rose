// {a} 
class sa_ { public: static int stat; int getstat() {return stat;} };

// {b} 
class sb_ { public: int i; int get25b(); sb_(int ii):i(ii){}};

// {c} 
class sc_ { public: int i; sc_(int ii) {i = ii;} };
extern sc_ objc_;

// {d} 
class sd_ { public: int i; sd_(int ii) {i = ii;} };
/* just provide decls */
extern sd_ fd_(sd_ obj) ;

// {e} 
class se_ { public: int i; se_(int ii) {i = ii;} };
class seb_ { public: se_ i; seb_(int ii) : i(ii) {} };

//  even this one is now external linkage
// class sf_ { public: int i; sf_(int ii) {i = ii;} };
// static sf_ objf_ (256);

extern void fb_();	// verify externally-linked classes and objs
int sa_::stat = 251;
int sb_::get25b() {return 252;}
sb_ objb_ (0);
sc_ objc_ (0);
sd_ objd_ (0);
se_ obje_ (0);

class sf_ { public: int i; sf_(int ii) {i = ii;} };
static sf_ objf_ (0);

// sg_ objg_ = {35};


void foobar()
   {
     fb_();		// verify externally-linked classes and objs
  // ieq(objf_.i, 0);
   }

