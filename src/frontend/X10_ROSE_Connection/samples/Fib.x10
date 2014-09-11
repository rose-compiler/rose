public class Fib {

public static def fib(n:long) {
    if (n<2) return 1;
    val f1:long;
    val f2:long;
    finish {
      async { f1 = fib(n-1); }
      f2 = fib(n-2);
    }
    return f1 + f2;
}

public static def main(args:Rail[String]) {
    val f:long;
    f = fib(10);
  }
}

