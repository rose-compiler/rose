#include <stdlib.h>
struct model ;

struct model 
{
    int i_, j_;
    float f_;
    int a_[1000];
}



void event0(struct model* m)
{
    {
        // Saving m->i_ is a bit better than saving i here
        int i = rand();
        m->i_ += i;
    }
    {
        // Saving i is a bit better than saving both m->i_ and m->j_
        int i = rand();
        m->i_ += i;
        m->j_ += i * 2; // any expression which only uses i.
    }
    {
        // Saving i takes 32 bits space, but needs to implementation the loop.
        // Saving m->i_ and m->j_ takes 64 bits, but save the loop.
        // Which scheme is chosen depends on the loop number?
        int i = rand();
        for (int j = 0; j < 100; ++j)
        {
            m->i_ += i;
            m->j_ += i;
        }
    }
    {
        // Only have to save once.
        m->i_ = 10;
        m->i_ = 10;
    }
    {
        // The reverse of the following code is:
        //   m->j_ = m->i_;
        //   m->i_ = pop();
        // Value graph analysis?
        m->i_ = m->j_;
        m->j_ = 10;
    }
    {
        // The following code equals to 
        //   m->j_ += m->i_ + 1;
        int i = m->i_;
        int j = i + 1;
        m->j_ += j;
    }
    {
        int i = m->i_;
        int j = i + 2;
    }
    {
        int i, j;
        // Although the following expression can be reversed, it needs two 
        // local variables to be saved. The better solution is only saving
        // m->i_.
        m->i_ += i + j;
    }
    {
        // The second statement has two reverse versions: m->i_ = pop() or m->i_ -= m->j_. 
        // But the second one has a prerequisite which needs the reverse of the third statement.
        // We can either save m->j_ twice, or m->i_ and m->j_ once separately.
        m->j_ = rand();
        m->i_ += m->j_;
        // m->k_ += m->j_;
        m->j_ = rand();
    }

}

void event1(struct model* m)
{
    // If m->i_ is large, saving m->j_ saves more space
    //
    // *********************************
    //     Option 1: Save branch flags.
    //     Space:    m->i_ bits
    //
    //     Option 2: Save m->j_
    //     Space:    sizeof(m->j_)
    // *********************************
    //
    for (int i = 0; i < m->i_; ++i)
    {
        if (rand() > RAND_MAX / 2)
            ++m->j_;
        else
            --m->j_;
    }
}

void event2(struct model* m)
{
    int i = rand() % 3;

    switch(i)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break
        default:
                break;
    }

    if (i > 2)
    {

    }

    for (i = 0; i < 10; ++i)
    {
        m->i_ += 2;
    }
}


class Model
{
    int i;
    int j;

    void event0()
    {
        i = 0;
        if ()
            event1();
    }
     
    void event0_save()
    {
        save(i);
    }

    void event0_fwd()
    {
        event0_save();
        event1_save();
    }

    void event1()
    {
        j = 0;
        if ()
            event0();
    }
};
