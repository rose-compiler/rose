// Example code from VX Works

#ifndef CPU
// DQ: changed defalt from Sparc to PPC
// #define	CPU 10
#define	CPU 91
#endif

#include "vxWorks.h"
#include "taskLib.h"
#include "vxwSemLib.h"

// Control the number of creatures in the pond

#define MAX_FLIES 5   // The initial number of flies
#define MAX_FROGS 2   // Same for frogs
#define FROG_STARVED 3     // How long ot takes a frog to starve
#define GRAB_CHANCE RAND_MAX / 2   // The probability of a frog catching a fly
#define FLY_BIRTH_RATE RAND_MAX / 4  // The probability of a fly reproducing
#define FROG_BIRTH_RATE RAND_MAX / 2     // Remeber here that frogs only give birth after they have eaten!!

int fly_lifecycle();
int frog_lifecycle();


/****************************************************************************************/
/* Linked list thingy                                                                   */

class node
{ 
public:
class Fly *key; 
node *next;

  node()
    {
      key = NULL;
      next = NULL;
    }
  ~node()
    {
      key = NULL;
      next = NULL;
    }
};

class List
{
private:
   node header;
   node end;
public:
  List()
    {
      header.key = NULL;
      header.next = &end;
      
      end.key = NULL;
      end.next = &end;
      
    }
  void add(Fly *val)
    {
      node *tmp;
      
      //      printf("\nAdding %X ",val);
      tmp = new node;

      tmp->next = header.next;
      tmp->key = val;

      header.next = tmp;
      //traverse();
    }

  void remove(Fly *val)
    {
      node *previous, *current;
      
      //printf("\nRemoving %X ",val);
      previous = &header;
      current = previous->next;
      
      
      while(current->key != val && current != &end)
	{
	  previous = current;
	  current = current->next;
	}
      
      if(current->key == val)
	{
	  previous->next = current->next;
	  delete current;
	}
      //traverse();

    }
  Fly *get()
    {
      Fly *tmp;

      tmp = header.next->key;
      
      //printf("\nGetting %X ",tmp);
      //traverse();
      return tmp;
    }

  int empty()
    {
      if(header.next == &end)
	return 1;
      else
	return 0;
    }
  
  void traverse()
      {
	node *current;
	
	//printf("\n");
	
	current = header.next;
	
	while(current != &end)
	  {
	    // printf("%X ",current->key);
	    current = current->next;
	  }
      }
};

/*****************************************************************************************/
/* The pond class is basically a database or resource. Both flies and frogs will access  */
/* the data and therefore proection is needed                                            */

class Pond
{
private:
  int number_of_flies, number_of_frogs;
  List *flying_db;
public:

  VXWBSem *fly_mutex;
  //mutex_t fly_mutex;
  Pond()
    {
      //     mutex_init(&fly_mutex,NULL,NULL);
      fly_mutex = new VXWBSem(SEM_Q_PRIORITY,SEM_FULL);
      number_of_flies = 0;
      number_of_frogs = 0;
      flying_db = new List;
    }
  void lock()
    // { mutex_lock(&fly_mutex) }
    { fly_mutex->take(WAIT_FOREVER); }

  void unlock()
    //    { mutex_unlock(&fly_mutex);}
    { fly_mutex->give(); }

  void fly_born()
    {
      number_of_flies++;
    }
  
  void fly_flying(Fly *id)
    { 
      flying_db->add(id);
    }
  
  void fly_landed(Fly *id)
    {
      flying_db->remove(id);    
    }
  
  /*  void fly_died()
    {   
      number_of_flies--;   
    }
    */
  void fly_died(Fly *id)
    {       
      number_of_flies--;
      flying_db->remove(id);   
    }
  
  Fly *flies_about()
    {   
      if(!flying_db->empty())
	return flying_db->get();
      else
	return NULL;
    }
};

/*************************************************************************************/
/* */

class Fly
{
private:
  enum { landed, flying, dead} status;
  int age;
  Pond *our_pond;
  VXWMSem *mutex;
  //  mutex_t mutex;
public:
  Fly(Pond *a_pond)
    {
      age = 3;
      status = landed;
      our_pond = a_pond;
     
      our_pond->lock();
      our_pond->fly_born();
      our_pond->unlock();

     mutex = new VXWMSem(SEM_Q_PRIORITY);
     //mutex_init(&mutex,NULL,NULL); 
    }

  void lock()
    // { mutex_lock(&mutex); }
    { mutex->take(WAIT_FOREVER); }

  void unlock()
    //{ mutex_unlock(&mutex); }
    { mutex->give(); }

  void fly_landed()
    {
     status = landed;
      our_pond->lock();
      our_pond->fly_landed(this);
      our_pond->unlock();
    }
  void fly_flying()
    { 
         status = flying;
	 our_pond->lock();
	 our_pond->fly_flying(this); // This will be change to a pointer to the fly shortly     
	 our_pond->unlock();
    }
  void fly_dead()
    {
      status = dead;
      our_pond->lock();
      our_pond->fly_died(this);   
      our_pond->unlock();
    }

// DQ: Added return type.
// fly_isdead()
int fly_isdead()
    {
      if(status == dead)
	return 1;
      else
	return 0;
    }

  int get_age()
    { return age; }
  void happy_birthday()
    { age--; }
  
  int grab()
    {
      int result;
      
      if(status==flying && (rand() > GRAB_CHANCE))
	{
	  status = dead;
	  our_pond->lock();
	  our_pond->fly_died(this);
	  our_pond->unlock();
	  //fly_dead();    // Here I make use of implied 'this' and af the recursive mutual exclusion pointer
	  result = 1;
	}
      else
	{
	  result = 0;
	}
      
      return result;
    }
};
/****************************************************************************/
/* This is the frog class */

class Frog
{
private:
  int last_meal;
  Pond *our_pond;
public:
    Frog(Pond *a_pond)
    {
      last_meal = 0;
      our_pond = a_pond;
    }

// DQ: Added return type.
// starved()
int starved()
    {
      if(last_meal>FROG_STARVED)
	return 1;
      else
	return 0;
    }
  void not_hungry()
    {
      last_meal = 0;
    }
  Fly *flies_about()
    {
      Fly *dummy;
      
      our_pond->lock();
      dummy =  our_pond->flies_about();
      our_pond->unlock();
      
      return dummy;
    }
  void no_dinner()
    {
      last_meal++;
    }

  void dead()
    {
    }
};
/****************************************************************************/
/* This is the controlling code - 'main' function                           */


Pond *the_pond;

// DQ: Added return type.
// ostmain()
int ostmain()
{
  //thread_t tid[MAX_FROGS+MAX_FLIES];
  //void *arg=NULL;
  int i;
  the_pond = new Pond;

   for( i=0; i<MAX_FLIES; i++)
    {
   // taskSpawn(  NULL, 100, 0, 2000, fly_lifecycle, 0,0,0,0,0,0,0,0,0,0);
      taskSpawn(  NULL, 100, 0, 2000, &fly_lifecycle, 0,0,0,0,0,0,0,0,0,0);
      //thr_create(NULL, NULL, fly_lifecycle, arg, NULL, &tid[i]);

    }

   for(i=0; i<MAX_FROGS; i++)
     {
       taskSpawn(  NULL, 100, 0, 2000, frog_lifecycle, 0,0,0,0,0,0,0,0,0,0);
       //thr_create(NULL, NULL, frog_lifecycle, arg, NULL, &tid[MAX_FLIES+i]);
     }

   //while(thr_join(NULL,NULL,NULL)==0);  
    
    return 0;

}

/***********************************************************************************/
/* Task that models/controls the life of the fly                                   */
/*                                                                                 */
/***********************************************************************************/

int fly_lifecycle()
{
  Fly fly(the_pond);
  int id;
  //thread_t *tid;
  //void *arg=NULL;

    
  // This is the life cycle - each time round the fly gets older 

  while(fly.get_age())
    {
      // the fly may have been eaten before it lands
      
      fly.lock();
      if(!fly.fly_isdead())
      	{
	  fly.unlock();

	  fly.lock();
	  fly.fly_landed();
	  fly.unlock();
	  
	  //	  sleep(1);
	  
	  fly.lock();
	  fly.fly_flying();
	  fly.unlock();

	    if(rand() < FLY_BIRTH_RATE)
	      {
		taskSpawn(  NULL, 100, 0, 2000, fly_lifecycle, 0,0,0,0,0,0,0,0,0,0);
		//tid = new thread_t;
		//thr_create(NULL, NULL, fly_lifecycle, arg, NULL, tid);
	      }

	    //sleep(1);
	  
	  fly.lock();
	  fly.happy_birthday();
	  fly.unlock();
	}
      else
	{
	  fly.unlock();
	  return NULL;
	}
    }
  fly.lock();
  fly.fly_dead();
  fly.unlock();

  return NULL;
}


  
/***********************************************************************************/
/* Task that models/controls the life of the fro                                   */
/*                                                                                 */
/***********************************************************************************/

int frog_lifecycle()
{
  Frog frog(the_pond);
  Fly *food;
  //thread_t *tid;
  //void *arg=NULL;

  while(!frog.starved())
    {
      
      
      if((food = (Fly *)frog.flies_about()) != NULL)
	{
	  food->lock();
	  if(food->grab()==1)          // try to catch a fly and eat it. Then think about giving birth
	    {
	      frog.not_hungry();
	      if(rand() < FROG_BIRTH_RATE)
		{
		  taskSpawn(  NULL, 100, 0, 2000, frog_lifecycle, 0,0,0,0,0,0,0,0,0,0);
		  //  tid = new thread_t;
		  //thr_create(NULL, NULL, frog_lifecycle, arg, NULL, tid);
		}
	    }
	  else
	    frog.no_dinner();
	  food->unlock();
	}
      else
	{
	  // The frog gets closer to starving

	  frog.no_dinner();

	
	}
      //      sleep(2);
    }
  frog.dead();
  return NULL;
}














