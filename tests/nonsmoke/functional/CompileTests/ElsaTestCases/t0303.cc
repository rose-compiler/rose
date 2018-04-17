// t0303.cc
// more issues with polymorphic operator returns

// from Mozilla inDOMUtils.i


struct nsStyleStructID {
  operator int () const;
};

struct nsCachedStyleData
{
  struct StyleStructInfo {
    bool    mIsReset;
  };

  static StyleStructInfo gInfo[];

  static bool IsReset(const nsStyleStructID& aSID) {
    return gInfo[aSID].mIsReset;
  };

  static bool IsReset2(const nsStyleStructID& aSID) {
    return (*(gInfo+aSID)).mIsReset;
  };
};
