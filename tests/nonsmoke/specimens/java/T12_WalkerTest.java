interface T12_Walker {
    public void handleNode(T12_Node n);
    public int getValue();
}

interface T12_Node {
    public void visit(T12_Walker v);
}

class T12_Inner implements T12_Node {
    T12_Node lhs,rhs;

    public T12_Inner(T12_Node l, T12_Node r) {
        lhs = l;
        rhs = r;
    }

    public void visit(T12_Walker v) {
        v.handleNode(lhs);
        v.handleNode(rhs);
    }
}

class T12_Leaf implements T12_Node {
    int value;
    public T12_Leaf(int v) {
        value = v;
    }

    public void visit(T12_Walker v) {
    }
}

class T12_Sum_Walker implements T12_Walker {
    int total;

    public T12_Sum_Walker() {
        total = 0;
    }

    public int getValue() {
        return total;
    }

    public void handleNode(T12_Node n) {
        if (n instanceof T12_Leaf) {
            T12_Leaf l = (T12_Leaf)n;
            total += l.value;
        } else {
            n.visit(this);
        }
    }
}

class T12_Max_Walker implements T12_Walker {
    int max;

    public T12_Max_Walker() {
        max = Integer.MIN_VALUE;
    }

    public int getValue() {
        return max;
    }

    public void handleNode(T12_Node n) {
        if (n instanceof T12_Leaf) {
            T12_Leaf l = (T12_Leaf)n;
            if (max < l.value) {
                max = l.value;
            }
        } else {
            n.visit(this);
        }
    }
}

public class T12_WalkerTest {
    public static void main(String args[]) {
        //
        //     inner
        //     /  \
        //    12  inner
        //         / \
        //        6   2
        //
        // max = 12
        // sum = 20
        //
        T12_Node tree = new T12_Inner(new T12_Leaf(12), new T12_Inner(new T12_Leaf(6), new T12_Leaf(2)));

        T12_Sum_Walker sum = new T12_Sum_Walker();
        T12_Max_Walker max = new T12_Max_Walker();

        sum.handleNode(tree);
        max.handleNode(tree);

        System.out.println(sum.getValue());
        System.out.println(max.getValue());
    }
}
