
// This is a bug found by DQ on (8/30/2004):
// Within the following code the "template<> struct numeric_limits<bool>"
// is defined to have a ClassDefinition body so that the members have a
// parent which is a SgClassDefinition, do we not have a SgTemplateDefinition
// class and if so isn't this an error?
// We do have a SgTemplateInstantiationDefn so it seems that this is what should have
// been used here!  So I think that this is a bug!

// To see the bug, look at the parent of the members of the 
// "template<> struct numeric_limits<bool>", they are presently
// members of a SgClassDefinition and I think thye should be
// members of a SgTemplateInstantiationDefn, which is derived from
// and SgClassDefinition.  It is not a serious bug, but it does appear to
// be a bug in the AST!

// 2nd bug: in file info!
// The instantiated class is instatiated 15 times in "limits"
// each time if the name of a function is repeated in a different 
// class then the file info refers to the first instance in the 
// first class it was listed.

// This is code from the GNU header file (ROSE/g++_HEADERS/hdrs1/limits)
  template<typename _Tp>
    struct numeric_limits : public __numeric_limits_base
    {
      static _Tp min() throw() { return static_cast<_Tp>(0); }
      static _Tp max() throw() { return static_cast<_Tp>(0); }
      static _Tp epsilon() throw() { return static_cast<_Tp>(0); }
      static _Tp round_error() throw() { return static_cast<_Tp>(0); }
      static _Tp infinity() throw()  { return static_cast<_Tp>(0); }
      static _Tp quiet_NaN() throw() { return static_cast<_Tp>(0); }
      static _Tp signaling_NaN() throw() { return static_cast<_Tp>(0); }
      static _Tp denorm_min() throw() { return static_cast<_Tp>(0); }
    };

  // Now there follow 15 explicit specializations.  Yes, 15.  Make sure
  // you get the count right.
  template<>
    struct numeric_limits<bool>
    {
      static const bool is_specialized = true;

      static bool min() throw()
      { return false; }
      static bool max() throw()
      { return true; }

      static const int digits = 1;
      static const int digits10 = 0;
      static const bool is_signed = false;
      static const bool is_integer = true;
      static const bool is_exact = true;
      static const int radix = 2;
      static bool epsilon() throw()
      { return false; }
      static bool round_error() throw()
      { return false; }

      static const int min_exponent = 0;
      static const int min_exponent10 = 0;
      static const int max_exponent = 0;
      static const int max_exponent10 = 0;

      static const bool has_infinity = false;
      static const bool has_quiet_NaN = false;
      static const bool has_signaling_NaN = false;
      static const float_denorm_style has_denorm = denorm_absent;
      static const bool has_denorm_loss = false;

      static bool infinity() throw()
      { return false; }
      static bool quiet_NaN() throw()
      { return false; }
      static bool signaling_NaN() throw()
      { return false; }
      static bool denorm_min() throw()
      { return false; }

      static const bool is_iec559 = false;
      static const bool is_bounded = true;
      static const bool is_modulo = false;

      // It is not clear what it means for a boolean type to trap.
      // This is a DR on the LWG issue list.  Here, I use integer
      // promotion semantics.
      static const bool traps = __glibcpp_integral_traps;
      static const bool tinyness_before = false;
      static const float_round_style round_style = round_toward_zero;
    };
