#include <stdlib.h>
struct model ;

struct model 
{
  int m_int;
  int m_int_array[1000];
}

;

void event0(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    ++m -> m_int;
}


void event1(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    ++m -> m_int;
  else 
    --m -> m_int;
}


void event2(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    --m -> m_int;
}


void event3(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    --m -> m_int;
  else 
    m -> m_int++;
}


void event4(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int++;
}


void event5(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int++;
  else 
    m -> m_int--;
}


void event6(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int--;
}


void event7(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int--;
  else 
    m -> m_int = i;
}


void event8(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int = i;
}


void event9(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int = i;
  else 
    m -> m_int += i;
}


void event10(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int += i;
}


void event11(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int += i;
  else 
    m -> m_int -= i;
}


void event12(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int -= i;
}


void event13(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int -= i;
  else 
    m -> m_int *= i;
}


void event14(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int *= i;
}


void event15(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int *= i;
  else 
    m -> m_int + i;
}


void event16(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int + i;
}


void event17(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int + i;
  else 
    (m -> m_int , i);
}


void event18(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    (m -> m_int , i);
}


void event19(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    (m -> m_int , i);
  else 
    m -> m_int == i;
}


void event20(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int == i;
}


void event21(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    m -> m_int == i;
  else 
    rand();
}


void event22(struct model *m)
{
  int i = ++m -> m_int;
  if (++m -> m_int) 
    --m -> m_int;
}


void event23(struct model *m)
{
  int i = ++m -> m_int;
  if (++m -> m_int) 
    --m -> m_int;
  else 
    m -> m_int++;
}


void event24(struct model *m)
{
  int i = ++m -> m_int;
  if (--m -> m_int) 
    m -> m_int++;
}


void event25(struct model *m)
{
  int i = ++m -> m_int;
  if (--m -> m_int) 
    m -> m_int++;
  else 
    m -> m_int--;
}


void event26(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int++) 
    m -> m_int--;
}


void event27(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int++) 
    m -> m_int--;
  else 
    m -> m_int = i;
}


void event28(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int--) 
    m -> m_int = i;
}


void event29(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int--) 
    m -> m_int = i;
  else 
    m -> m_int += i;
}


void event30(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int = i) 
    m -> m_int += i;
}


void event31(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int = i) 
    m -> m_int += i;
  else 
    m -> m_int -= i;
}


void event32(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int += i) 
    m -> m_int -= i;
}


void event33(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int += i) 
    m -> m_int -= i;
  else 
    m -> m_int *= i;
}


void event34(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int -= i) 
    m -> m_int *= i;
}


void event35(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int -= i) 
    m -> m_int *= i;
  else 
    m -> m_int + i;
}


void event36(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int *= i) 
    m -> m_int + i;
}


void event37(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int *= i) 
    m -> m_int + i;
  else 
    (m -> m_int , i);
}


void event38(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int + i) 
    (m -> m_int , i);
}


void event39(struct model *m)
{
  int i = ++m -> m_int;
  if (m -> m_int + i) 
    (m -> m_int , i);
  else 
    m -> m_int == i;
}


void event40(struct model *m)
{
  int i = ++m -> m_int;
  if ((m -> m_int , i)) 
    m -> m_int == i;
}


void event41(struct model *m)
{
  int i = ++m -> m_int;
  if ((m -> m_int , i)) 
    m -> m_int == i;
  else 
    rand();
}


void event42(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      ++m -> m_int;
}


void event43(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      ++m -> m_int; else {}
  else if (rand() > 1073741823) 
    ++m -> m_int;
  else 
    --m -> m_int;
}


void event44(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      ++m -> m_int;
    else 
      --m -> m_int;
}


void event45(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      ++m -> m_int;
    else 
      --m -> m_int;
  else if (rand() > 1073741823) 
    --m -> m_int;
}


void event46(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      --m -> m_int;
}


void event47(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      --m -> m_int; else {}
  else if (rand() > 1073741823) 
    --m -> m_int;
  else 
    m -> m_int++;
}


void event48(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      --m -> m_int;
    else 
      m -> m_int++;
}


void event49(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      --m -> m_int;
    else 
      m -> m_int++;
  else if (rand() > 1073741823) 
    m -> m_int++;
}


void event50(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int++;
}


void event51(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int++; else {}
  else if (rand() > 1073741823) 
    m -> m_int++;
  else 
    m -> m_int--;
}


void event52(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int++;
    else 
      m -> m_int--;
}


void event53(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int++;
    else 
      m -> m_int--;
  else if (rand() > 1073741823) 
    m -> m_int--;
}


void event54(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int--;
}


void event55(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int--; else {}
  else if (rand() > 1073741823) 
    m -> m_int--;
  else 
    m -> m_int = i;
}


void event56(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int--;
    else 
      m -> m_int = i;
}


void event57(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int--;
    else 
      m -> m_int = i;
  else if (rand() > 1073741823) 
    m -> m_int = i;
}


void event58(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int = i;
}


void event59(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int = i; else {}
  else if (rand() > 1073741823) 
    m -> m_int = i;
  else 
    m -> m_int += i;
}


void event60(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int = i;
    else 
      m -> m_int += i;
}


void event61(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int = i;
    else 
      m -> m_int += i;
  else if (rand() > 1073741823) 
    m -> m_int += i;
}


void event62(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int += i;
}


void event63(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int += i; else {}
  else if (rand() > 1073741823) 
    m -> m_int += i;
  else 
    m -> m_int -= i;
}


void event64(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int += i;
    else 
      m -> m_int -= i;
}


void event65(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int += i;
    else 
      m -> m_int -= i;
  else if (rand() > 1073741823) 
    m -> m_int -= i;
}


void event66(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int -= i;
}


void event67(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int -= i; else {}
  else if (rand() > 1073741823) 
    m -> m_int -= i;
  else 
    m -> m_int *= i;
}


void event68(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int -= i;
    else 
      m -> m_int *= i;
}


void event69(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int -= i;
    else 
      m -> m_int *= i;
  else if (rand() > 1073741823) 
    m -> m_int *= i;
}


void event70(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int *= i;
}


void event71(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int *= i; else {}
  else if (rand() > 1073741823) 
    m -> m_int *= i;
  else 
    m -> m_int + i;
}


void event72(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int *= i;
    else 
      m -> m_int + i;
}


void event73(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int *= i;
    else 
      m -> m_int + i;
  else if (rand() > 1073741823) 
    m -> m_int + i;
}


void event74(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int + i;
}


void event75(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int + i; else {}
  else if (rand() > 1073741823) 
    m -> m_int + i;
  else 
    (m -> m_int , i);
}


void event76(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int + i;
    else 
      (m -> m_int , i);
}


void event77(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int + i;
    else 
      (m -> m_int , i);
  else if (rand() > 1073741823) 
    (m -> m_int , i);
}


void event78(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      (m -> m_int , i);
}


void event79(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      (m -> m_int , i); else {}
  else if (rand() > 1073741823) 
    (m -> m_int , i);
  else 
    m -> m_int == i;
}


void event80(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      (m -> m_int , i);
    else 
      m -> m_int == i;
}


void event81(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      (m -> m_int , i);
    else 
      m -> m_int == i;
  else if (rand() > 1073741823) 
    m -> m_int == i;
}


void event82(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int == i;
}


void event83(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int == i; else {}
  else if (rand() > 1073741823) 
    m -> m_int == i;
  else 
    rand();
}


void event84(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int == i;
    else 
      rand();
}


void event85(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (rand() > 1073741823) 
      m -> m_int == i;
    else 
      rand();
  else if (++m -> m_int) 
    --m -> m_int;
}


void event86(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (++m -> m_int) 
      --m -> m_int;
}


void event87(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (++m -> m_int) 
      --m -> m_int; else {}
  else if (++m -> m_int) 
    --m -> m_int;
  else 
    m -> m_int++;
}


void event88(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (++m -> m_int) 
      --m -> m_int;
    else 
      m -> m_int++;
}


void event89(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (++m -> m_int) 
      --m -> m_int;
    else 
      m -> m_int++;
  else if (--m -> m_int) 
    m -> m_int++;
}


void event90(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (--m -> m_int) 
      m -> m_int++;
}


void event91(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (--m -> m_int) 
      m -> m_int++; else {}
  else if (--m -> m_int) 
    m -> m_int++;
  else 
    m -> m_int--;
}


void event92(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (--m -> m_int) 
      m -> m_int++;
    else 
      m -> m_int--;
}


void event93(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (--m -> m_int) 
      m -> m_int++;
    else 
      m -> m_int--;
  else if (m -> m_int++) 
    m -> m_int--;
}


void event94(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int++) 
      m -> m_int--;
}


void event95(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int++) 
      m -> m_int--; else {}
  else if (m -> m_int++) 
    m -> m_int--;
  else 
    m -> m_int = i;
}


void event96(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int++) 
      m -> m_int--;
    else 
      m -> m_int = i;
}


void event97(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int++) 
      m -> m_int--;
    else 
      m -> m_int = i;
  else if (m -> m_int--) 
    m -> m_int = i;
}


void event98(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int--) 
      m -> m_int = i;
}


void event99(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int--) 
      m -> m_int = i; else {}
  else if (m -> m_int--) 
    m -> m_int = i;
  else 
    m -> m_int += i;
}


void event100(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int--) 
      m -> m_int = i;
    else 
      m -> m_int += i;
}


void event101(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int--) 
      m -> m_int = i;
    else 
      m -> m_int += i;
  else if (m -> m_int = i) 
    m -> m_int += i;
}


void event102(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int = i) 
      m -> m_int += i;
}


void event103(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int = i) 
      m -> m_int += i; else {}
  else if (m -> m_int = i) 
    m -> m_int += i;
  else 
    m -> m_int -= i;
}


void event104(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int = i) 
      m -> m_int += i;
    else 
      m -> m_int -= i;
}


void event105(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int = i) 
      m -> m_int += i;
    else 
      m -> m_int -= i;
  else if (m -> m_int += i) 
    m -> m_int -= i;
}


void event106(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int += i) 
      m -> m_int -= i;
}


void event107(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int += i) 
      m -> m_int -= i; else {}
  else if (m -> m_int += i) 
    m -> m_int -= i;
  else 
    m -> m_int *= i;
}


void event108(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int += i) 
      m -> m_int -= i;
    else 
      m -> m_int *= i;
}


void event109(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int += i) 
      m -> m_int -= i;
    else 
      m -> m_int *= i;
  else if (m -> m_int -= i) 
    m -> m_int *= i;
}


void event110(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int -= i) 
      m -> m_int *= i;
}


void event111(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int -= i) 
      m -> m_int *= i; else {}
  else if (m -> m_int -= i) 
    m -> m_int *= i;
  else 
    m -> m_int + i;
}


void event112(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int -= i) 
      m -> m_int *= i;
    else 
      m -> m_int + i;
}


void event113(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int -= i) 
      m -> m_int *= i;
    else 
      m -> m_int + i;
  else if (m -> m_int *= i) 
    m -> m_int + i;
}


void event114(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int *= i) 
      m -> m_int + i;
}


void event115(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int *= i) 
      m -> m_int + i; else {}
  else if (m -> m_int *= i) 
    m -> m_int + i;
  else 
    (m -> m_int , i);
}


void event116(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int *= i) 
      m -> m_int + i;
    else 
      (m -> m_int , i);
}


void event117(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int *= i) 
      m -> m_int + i;
    else 
      (m -> m_int , i);
  else if (m -> m_int + i) 
    (m -> m_int , i);
}


void event118(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int + i) 
      (m -> m_int , i);
}


void event119(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int + i) 
      (m -> m_int , i); else {}
  else if (m -> m_int + i) 
    (m -> m_int , i);
  else 
    m -> m_int == i;
}


void event120(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int + i) 
      (m -> m_int , i);
    else 
      m -> m_int == i;
}


void event121(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if (m -> m_int + i) 
      (m -> m_int , i);
    else 
      m -> m_int == i;
  else if ((m -> m_int , i)) 
    m -> m_int == i;
}


void event122(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if ((m -> m_int , i)) 
      m -> m_int == i;
}


void event123(struct model *m)
{
  int i = ++m -> m_int;
  if (rand() > 1073741823) 
    if ((m -> m_int , i)) 
      m -> m_int == i; else {}
  else if ((m -> m_int , i)) 
    m -> m_int == i;
  else 
    rand();
}

