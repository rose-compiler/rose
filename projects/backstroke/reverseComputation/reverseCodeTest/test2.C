#include <stdlib.h>
struct model ;

struct model 
{
  int i_;
  int a_[1000];
}

;

void event0(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(++m -> model::i_);
  }
}


void event1(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_++);
  }
}


void event2(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(!m -> model::i_);
  }
}


void event3(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = 290);
  }
}


void event4(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && 290);
  }
}


void event5(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || 290);
  }
}


void event6(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = ++m -> model::i_);
  }
}


void event7(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ++m -> model::i_);
  }
}


void event8(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ++m -> model::i_);
  }
}


void event9(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = m -> model::i_++);
  }
}


void event10(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && m -> model::i_++);
  }
}


void event11(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || m -> model::i_++);
  }
}


void event12(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = !m -> model::i_);
  }
}


void event13(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && !m -> model::i_);
  }
}


void event14(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || !m -> model::i_);
  }
}


void event15(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = m -> model::i_ = 3723);
  }
}


void event16(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && (m -> model::i_ = 3723));
  }
}


void event17(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || (m -> model::i_ = 3723));
  }
}


void event18(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = m -> model::i_ && 3723);
  }
}


void event19(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && (m -> model::i_ && 3723));
  }
}


void event20(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || m -> model::i_ && 3723);
  }
}


void event21(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = m -> model::i_ || 3723);
  }
}


void event22(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && (m -> model::i_ || 3723));
  }
}


void event23(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || (m -> model::i_ || 3723));
  }
}


void event24(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (++m -> model::i_?++m -> model::i_ : m -> model::i_++));
  }
}


void event25(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((++m -> model::i_?++m -> model::i_ : m -> model::i_++)));
  }
}


void event26(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((++m -> model::i_?++m -> model::i_ : m -> model::i_++)));
  }
}


void event27(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (rand() > 1073741823?++m -> model::i_ : m -> model::i_++));
  }
}


void event28(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((rand() > 1073741823?++m -> model::i_ : m -> model::i_++)));
  }
}


void event29(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((rand() > 1073741823?++m -> model::i_ : m -> model::i_++)));
  }
}


void event30(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (++m -> model::i_?++m -> model::i_ : (m -> model::i_ = 3723)));
  }
}


void event31(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((++m -> model::i_?++m -> model::i_ : (m -> model::i_ = 3723))));
  }
}


void event32(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((++m -> model::i_?++m -> model::i_ : (m -> model::i_ = 3723))));
  }
}


void event33(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (rand() > 1073741823?++m -> model::i_ : (m -> model::i_ = 3723)));
  }
}


void event34(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((rand() > 1073741823?++m -> model::i_ : (m -> model::i_ = 3723))));
  }
}


void event35(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((rand() > 1073741823?++m -> model::i_ : (m -> model::i_ = 3723))));
  }
}


void event36(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (++m -> model::i_?m -> model::i_++ : (m -> model::i_ = 3723)));
  }
}


void event37(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((++m -> model::i_?m -> model::i_++ : (m -> model::i_ = 3723))));
  }
}


void event38(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((++m -> model::i_?m -> model::i_++ : (m -> model::i_ = 3723))));
  }
}


void event39(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (rand() > 1073741823?m -> model::i_++ : (m -> model::i_ = 3723)));
  }
}


void event40(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((rand() > 1073741823?m -> model::i_++ : (m -> model::i_ = 3723))));
  }
}


void event41(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((rand() > 1073741823?m -> model::i_++ : (m -> model::i_ = 3723))));
  }
}


void event42(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (++m -> model::i_?m -> model::i_ && 3723 : m -> model::i_ || 3723));
  }
}


void event43(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((++m -> model::i_?m -> model::i_ && 3723 : m -> model::i_ || 3723)));
  }
}


void event44(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((++m -> model::i_?m -> model::i_ && 3723 : m -> model::i_ || 3723)));
  }
}


void event45(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ = (rand() > 1073741823?m -> model::i_ && 3723 : m -> model::i_ || 3723));
  }
}


void event46(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ && ((rand() > 1073741823?m -> model::i_ && 3723 : m -> model::i_ || 3723)));
  }
}


void event47(struct model *m)
{
{
    int i = ++m -> model::i_;
    srand(m -> model::i_ || ((rand() > 1073741823?m -> model::i_ && 3723 : m -> model::i_ || 3723)));
  }
}

