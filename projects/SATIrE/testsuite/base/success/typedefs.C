typedef class list *l_ptr;
class list { public: list *next; l_ptr ptr; };

void f()
{
    list *l;
    list *ll;
    l_ptr lll;
    list llll;
    struct list lllll;

    l = new list;
    ll = l;
    lll = ll;
    l->next = lll;
    lll = l->next;
    l->ptr = ll;
    ll = l->ptr;
    llll = *l;
    *l = lllll;
    lllll = *l;
    llll = lllll;
}
