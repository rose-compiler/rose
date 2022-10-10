int nglobal=10;
int sinit=0;
int i;
int j;

int main() {
  int n;
  n=nglobal;
  int z=n;
  int s=sinit;
  for(i=1;i<n;i++) {
    s=s+1;
    for(j=1;j<n;j++) {
      s=s+j;
    }
  }
  z=s;
  return 0;
}
