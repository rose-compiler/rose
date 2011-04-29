int main()
 { 
int k = 0;
int x = 2;
if (true && true && true || false) {
x = 2
;for (int i = 0; i < 5; i++) { 
for (int p = 0; p < 5; p++) { 
if (i == 2 || i == 3) {
k = 1;
}
else if (i == 3 && i == 4 && i == 5) {
k = 1;
}
else {
k = 2;
}
}
for (int q = 0; q < 5; q++) { 
k = 1;
}
}
}
else {
x = 2;
for (int i = 0; i < 5; i++) { 
k = 1;
for (int q = 0; q < 5; q++) { 
if (q == 2) {
k = 1;
}
k = 1;
}
}
}
return 0;
}
