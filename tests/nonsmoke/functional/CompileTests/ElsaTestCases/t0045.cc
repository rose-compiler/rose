// cc.in45
// inheritance with several keywords

class B
{};

class D1 : public virtual B
{};

class D2 : virtual public B
{};

class D3 : public B
{};

class D4 : virtual B
{};

class D5 : B
{};

