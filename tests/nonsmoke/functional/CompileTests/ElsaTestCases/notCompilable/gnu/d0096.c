// give only a warning when a function that was declared implicitly by
// a function call is later declared explicitly with a more specific
// and therefore (likely) inconsistent type; FIX: I don't think this
// is gnu specific, but is K&R specific

// sm: actually, it's ordinary C89

int main() {
  int buf[4];
  int const in[16];
  MD5_Transform(buf, in);
}

void MD5_Transform(int buf[4], int const in[16])
{
}
