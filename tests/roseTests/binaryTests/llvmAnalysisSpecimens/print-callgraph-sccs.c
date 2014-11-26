/* OPT: -print-callgraph-sccs */

void _start() {}

void leaf1() {}
void leaf2() {}
void leaf3() {}

void mid1() { leaf1(); }
void mid2() { leaf2(); }
void mid3() { leaf3(); }

void recursive() { recursive(); }

void mutrec2();
void mutrec1() { mutrec2(); }
void mutrec2() { mutrec1(); }
