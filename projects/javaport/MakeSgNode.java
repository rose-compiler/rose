package gov.llnl.casc.rose;

class MakeSgNode {
  public static SgNode makeDerivedJavaNodeClass(long cPtr, boolean cMemoryOwn) {
    if (cPtr == 0) return null;
    SgNode n = new SgNode(cPtr, false);
    VariantT v = n.variantT();
    if (v == VariantT.V_SgFunctionDeclaration) return new SgFunctionDeclaration(cPtr, cMemoryOwn);
    if (v == VariantT.V_SgClassDeclaration) return new SgClassDeclaration(cPtr, cMemoryOwn);
    return new SgNode(cPtr, cMemoryOwn);
  }
}
