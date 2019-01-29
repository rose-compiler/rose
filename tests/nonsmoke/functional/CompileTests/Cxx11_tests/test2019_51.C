// The minimized code for:
//   YYYYY: testTranslator[50041] 4.00701s Rose[FATAL]: .../EDG/edgRose/edgRose.C:59854


enum E {
  e = sizeof( (struct A*)0 )
};

// The issue is that a iek_src_seq_secondary_decl is found in EDG_ROSE_Translation::parse_enumerators.

// The original C++ code used a scoped enumeration:

enum class F {
  e = sizeof( (struct A*)0 )
};

// The JIRA issue is ROSE-1675
