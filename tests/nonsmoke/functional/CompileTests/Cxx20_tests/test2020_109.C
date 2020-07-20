for /*each base or member subobject o of T*/
  if (auto cmp = lhs.o <=> rhs.o; cmp != 0) return cmp;
return strong_ordering::equal; // converts to everything

