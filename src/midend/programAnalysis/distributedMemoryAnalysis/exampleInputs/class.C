template <class T>
class ListNode
{
public:
    ListNode(T t) : value(t) { }

    T value;
    ListNode<T> *next;
};

template <class T>
class List
{
public:
    List() : first(0) { }
    ListNode<T> *find(const T &t)
    {
        ListNode<T> *p = first;
        while (p != 0 && p->value != t)
            p = p->next;
        return p;
    }
    void insert(T t)
    {
        ListNode<T> *ln = new ListNode<T>(t);
        ln->next = first;
        first = ln;
    }
    ListNode<T> *head() const { return first; }

private:
    ListNode<T> *first;
};

template <class T>
bool operator!=(const List<T> &l1, const List<T> &l2)
{
    return (&l1 != &l2);
}

int il(void)
{
    List<int> il;
    ListNode<int> *p = il.find(42);
    return p->value;
}

namespace N
{
    float head(List<float> &fl)
    {
        return fl.head()->value;
    }
    
    bool eq(List<float> &f1, List<float> &f2)
    {
        ListNode<float> *p = f1.head();
        ListNode<float> *q = f2.head();
        while (p->value == q->value)
        {
            p = p->next;
            q = q->next;
        }
        return (p == 0 && q == 0);
    }
}

int headhead(List<List<int> > &il)
{
    ListNode<List<int> > *first = il.head();
    List<int> l = first->value;
    il.find(l);
    ListNode<int> *n = l.find(42);
    return n->value;
}
