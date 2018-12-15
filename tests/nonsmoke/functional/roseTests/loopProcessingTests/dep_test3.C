// velocity BC values
char *bcVelocity;
// left boundary nodes
int *bc3Nodes;
int numBC3Nodes;

void applyVelocityBC(char *u)
{
  for (int in = 0; in <= -1 + numBC3Nodes; in += 1) {
    u[bc3Nodes[in]] = bcVelocity[3];
  }
}
