
// Ghassan says that this generates a SgBasicBlock which has the SgIfStmt
// as a parent, but is not a child of the SgIfStmt IR node.
void checkGoto()
{
#error "AST has a SgBasicBlock which has the SgIfStmt as a parent, but is not a child of the SgIfStmt IR node."
    if (0) {
        return;
    }
}
