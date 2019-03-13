
// {a} 
class sa_ { public: static int stat; int getstat() {return stat;} };

// {b} 
class sb_ { public: int i; int get25b(); sb_(int ii):i(ii){}};

// {c} 
class sc_ { public: int i; sc_(int ii) {i = ii;} };
extern sc_ objc_;

// {d} 
class sd_ { public: int i; sd_(int ii) {i = ii;} };
/* provide definitions */
extern sd_ fd_(sd_ obj) { return obj; }

// {e} 
class se_ { public: int i; se_(int ii) {i = ii;} };
class seb_ { public: se_ i; seb_(int ii) : i(ii) {} };

//  even this one is now external linkage
// class sf_ { public: int i; sf_(int ii) {i = ii;} };
// static sf_ objf_ (256);

extern sb_ objb_;
extern sc_ objc_;
extern sd_ objd_;
extern se_ obje_;

class sf_ { public: int i; sf_(int ii) {i = ii;} }; // external
static sf_ objf_ (256);

// extern sg_ objg_;

extern void fb_()
	{
	extern const char * Filename;
	const char * oldFilename = Filename;
	Filename = __FILE__;
#if 0
	ieq(sa_::stat, 251);
	ieq(objb_.get25b(), 252);
	ieq(objc_.i, 0);
	ieq(objd_.i, 0);
	ieq(obje_.i, 0);
	ieq(objf_.i, 256);
#endif
	Filename = oldFilename;
	}

