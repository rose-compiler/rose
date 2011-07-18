public final class test2011_22 // BadInstanceOf 
   {
     private static class Animal {}
     private static final class Fish extends Animal { void swim(){}  }
     private static final class Spider extends Animal { void crawl(){} }

     public static void doSomething(Animal aAnimal)
        {
           if (aAnimal instanceof Fish)
             {
               Fish fish = (Fish)aAnimal;
               fish.swim();
             }
            else
               if (aAnimal instanceof Spider)
                  {
                    Spider spider = (Spider)aAnimal;
                    spider.crawl();
                  }
        }
   }
