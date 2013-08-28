int s272(float t)
{
float a[16];
#pragma SIMD
for (int i = 0; i < 16; i++) {
 a[i] = t;
}
return 0;
}

