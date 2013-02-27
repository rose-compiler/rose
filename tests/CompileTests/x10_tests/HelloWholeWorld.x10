// x10c++ -o HelloWholeWorld -O HelloWholeWorld.x10

import x10.io.Console;

class HelloWholeWorld {
    public static def main(Array[String]) {
        finish for (p in Place.places()) {
            async at (p) {
                async Console.OUT.println("Hello World from place " + p.id);
            }
        }
    }
}
