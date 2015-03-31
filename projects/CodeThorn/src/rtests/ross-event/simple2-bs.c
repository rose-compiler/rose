/* Definitions of types, global variables, and subroutines
   in the ROSS event processor */
#include <ross.h>

void dummy_func() {}
#include <iostream>


/* Desciption of the model this program implements:

   Particle spammer model
   In this model, each logical process (LP) has a set of
   particles, and each LP keeps track of how many particles
   it has (there is no other attributes of particles being stored;
   the particles are all imagined equivalent).
  
   The simulation proceeds by the LP's sending particles to
   each other.

   Whenever particles arrive at an LP, it sends some of its particles
   to a randomly chosen LP. The particles arrive to the destination
   at some future time, selected from a shiften exponential
   distribution.

   An event message contains one number, that is how many particles
   are arriving.

   The state of an LP has two numbers, how many particles it has at
   the moment, and the difference of the number of sent and received
   particles.
  
   The relevant pieces of code to change in order to modify the simulation
   model (or write a new on from scratch) are:

     1. The data structures for the event messages, and for the state
        of a logical process (LP).
 
     2. The initialization subroutine, that constructs the initial
        state of an LP.

     3. The finalization subroutine, which collects statistics,
        or otherwise process the final state of an LP when the
        simulation has ended.

     4. The event processing subroutine, which changes the state
        of an LP according to a given event. This subroutine may
        generate other events as a response to the incoming
        event.

     (5.) For optimistic simulation, a revrese event subroutine
          must be provided which is the inverse of the event
          processing subroutine. (E.g. gvien an event E, and the
          state S of an LP just after E was processed, change the
          S back to what it was just before E was processed.

   The statistics gathering of the LP's happens on a per processor basis
   (e.g. ono each MPI task). After the simulation has completed, this data
   on the different tasks must be accumulated to yield global statistics.
   In this code, this is done thorugh some MPI_Allreduce() function calls.
*/
   


/* Data structure to hold state of logical process (LP) */
typedef struct {
  long long int npart;
  long long int transit;
} ctr_state;

/* Data structure to hold contents of a message/event */
typedef struct {
  long long int	npart_send;
} ctr_message;

/* Global variable used to accumulate statistics. */
static struct {
  long long int npart_tot_init,npart_tot;
  long long int transit_tot;
  long long int lpcount;
} stat_data = { 0 , 0 , 0 };


/* Variable htat control time distribution of events */
static tw_stime lookahead = 1.0;
static tw_stime mean = 1.01;

/* Global variables */
static unsigned int nlp_per_pe = 8;
static unsigned int total_no_lps = 0;

/* Parameters to control how much memory for optimism is allocated */
static tw_stime mult = 1.4;
static int optimistic_memory = 10000;
static int start_events = 1;


/* Map a global logical process (LP) id to a physical processor (MPI task) */
static tw_peid ctr_map(tw_lpid gid) {
  return (tw_peid) gid / g_tw_nlp;
}

/* Initialize a logical process (LP) */
static void ctr_init(ctr_state *s, tw_lp *lp) {
  /* Decide how many particles this LP has initially */ {
    s->npart = (int) tw_rand_integer(lp->rng,0,10);
    s->transit = 0;
  }

  /* Create initial events, e.g. particles that this LP sends */ {
    int i;
    for(i = 0; i < start_events; i++) {
      /* Initial events consist of an LP sending particles to itself */
      int nsend = (int) tw_rand_integer(lp->rng,0,10);
      ctr_message *msg;
      tw_event *evt;
      s->transit += nsend;
      evt = tw_event_new(lp->gid,tw_rand_exponential(lp->rng, mean) + lookahead, 
			 lp);
      msg = (ctr_message*)tw_event_data(evt); // MS: added explicit cast
      msg->npart_send = nsend;
      tw_event_send(evt);
    }
  }

  /* Collect statistics on initial configuration */ {
    stat_data.npart_tot_init += s->npart + s->transit;
    stat_data.lpcount += 1;
  }
}

#include "Timer.h"
#ifdef BACKSTROKE_GENERATED
#include "runtime.h"
#include "rose_simple2-event-funcs-modified-2.c"
#else
/* Event processing routine. Perform the relevant actions when event
   m happens to object s. */
static void ctr_event_handler(ctr_state * s, tw_bf * bf, ctr_message * m, tw_lp * lp) {
  /* We received the message m. Update local particle count,
     and number of particles in transit by number of received particles. */ {
    s->npart += m->npart_send;
    s->transit -= m->npart_send;
  }

  /* Generate a new event to send some number of particles to
     another process (LP) */ {
    tw_lpid destination;
    double event_time;
    long long int nsend;
    ctr_message *msg;
    tw_event *evt;
    
    /* Let destination be a random other LP */
    destination = tw_rand_integer(lp->rng, 0, total_no_lps - 1);
    if(destination < 0 || destination >= (g_tw_nlp * tw_nnodes()))
      tw_error(TW_LOC, "bad dest");
    
    /* Pick a random numbe rof particles to send out */    
    nsend = tw_rand_integer(lp->rng,0,(s->npart+1)/2);
    
    /* Draw time at which sent out particles will arrive from exponential distribution */
    event_time = tw_rand_exponential(lp->rng, mean) + lookahead;
    
    /* Create a new event, fill in how many particles to send,
       and send it*/ {
      evt = tw_event_new(destination, event_time, lp);
      msg = (ctr_message*)tw_event_data(evt);
      msg->npart_send = nsend;
      tw_event_send(evt);
    }
    
    /* Update local particle count and number of particle sin transit */ {
      s->npart -= nsend;
      s->transit += nsend;
    }
    
    /* Store number of sent particles in auxiliary data field, that can be used by
       the inverse event routine to "undo" this event */ {
      *((unsigned int *) bf) = nsend;
    }
  }
}

/* Reverse event processing routine. Should perform the inverse of ctr_event_handler() */
static void ctr_event_handler_rc(ctr_state * s, tw_bf * bf, ctr_message * m, tw_lp * lp)
{
  /* We used three random numbers in forward event processing.
     Roll the random number generator back threee times */ {
    tw_rand_reverse_unif(lp->rng);
    tw_rand_reverse_unif(lp->rng);
    tw_rand_reverse_unif(lp->rng);
  }

  /* Use auxiliary data field saved by event to
     recaculate the particle counts we had */ {
    s->npart += *(unsigned int *) bf;
    s->transit -= *(unsigned int *) bf;
  }
  
  /* Update local particle counts based on number of particles that were send in
     message m */ {
    s->npart -= m->npart_send;
    s->transit += m->npart_send;
  }
}
#endif

/* Routine called after simulation finishes. Called for each LP,
   and can be used to collect statistics for each processor (MPI rank) */
static void ctr_finish(ctr_state * s, tw_lp * lp) {
  stat_data.npart_tot += s->npart;
  stat_data.transit_tot += s->transit;
}


int main(int argc, char *argv[]) {
  /* (***) I don't know whether this is necessary, but this was there in the
     code (phold model) I based this simulation on */ {
    // get rid of error if compiled w/ MEMORY queues
    g_tw_memory_nqueues=1;
  }

  // MS: provide C++ compatible ROSS macro for TWOPT_END
#define BS_TWOPT_END() {(tw_opttype)0,(const char*)0,(const char*)0,(void*)0}

  /* Define command line options specific to this application, parse
     them, and initialize the ROSS simulation engine (and MPI) */ {
    const tw_optdef app_opt[] =
      {
	TWOPT_GROUP("Particle spammer"),
	TWOPT_UINT("nlp", nlp_per_pe, "number of LPs per processor"),
	TWOPT_STIME("mean", mean, "exponential distribution mean for timestamps (>1.0)"),
	TWOPT_STIME("lookahead", lookahead, "lookahead for events (<=1.0)"),
	BS_TWOPT_END()
      };
    
    tw_opt_add(app_opt);
    tw_init(&argc, &argv);

    /* Check that user defined parameters are in range */ {
      if(lookahead > 1.0)
	tw_error(TW_LOC,"lookahead > 1.0;  needs to be less\n");
      if(mean <= 1.0)
	tw_error(TW_LOC,"mean <= 1.0; needs to be greater\n");
      mean = mean - lookahead;
    }
  }

  /* Don't know if necessary; see comment (***) above */ {
    g_tw_memory_nqueues = 16; // give at least 16 memory queue event
  }

  /* Compute total number of LP's (used to choose receivers) */
  total_no_lps = tw_nnodes() * g_tw_npe * nlp_per_pe;
  g_tw_events_per_pe = (mult * nlp_per_pe * start_events) + optimistic_memory;
  g_tw_lookahead = lookahead;

  /* Set up how many LP's on each processor */
  tw_define_lps(nlp_per_pe, sizeof(ctr_message), 0);

  /* Set types of LP's (e.g. define the initialization, event processing,
     reverese event , and finalization subroutines for each LP) */ {
    tw_lptype mylp_types[] = {
      { (init_f) ctr_init,
	(pre_run_f) dummy_func,
	(event_f) ctr_event_handler,
	(revent_f) ctr_event_handler_rc,
	(final_f) ctr_finish,
	(map_f) ctr_map,
	sizeof(ctr_state)
      },
      {0}
    };

    for(unsigned int i = 0; i < g_tw_nlp; i++)
      tw_lp_settype(i, &mylp_types[0]);
  }

  if(tw_ismaster()) {
    printf("========================================\n");
    printf("Particle spammer configuration..........\n");
    printf("   Lookahead..............%lf\n", lookahead);
    printf("   Mean...................%lf\n", mean);
    printf("   Mult...................%lf\n", mult);
    printf("   Memory.................%u\n", optimistic_memory);
    printf("\n");
    printf("   Number of MPI tasks...................%d\n", (int) tw_nnodes());
    printf("   Total number of LP's..................%d\n", (int) total_no_lps);
    
    printf("========================================\n\n");
  }

  Timer timer;
  double totalRunTime=0.0;
  if(tw_ismaster()) {
    timer.start();
    printf("* Starting simulation...\n");
  }
  MPI_Barrier(MPI_COMM_WORLD);
  tw_run();
  MPI_Barrier(MPI_COMM_WORLD);
  if(tw_ismaster()) {
    printf("* Simulation finished...\n");
    timer.stop();
    totalRunTime=timer.getElapsedTimeInSec();
  }
  
  /* Collect final particle counts and print statistics */ {
    double nloc0 = stat_data.npart_tot_init,ntot0;
    double nloc1 = stat_data.npart_tot,ntot1;
    double txloc = stat_data.transit_tot,txtot;
    double ploc = stat_data.lpcount,ptot;
    MPI_Allreduce(&nloc0,&ntot0,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&nloc1,&ntot1,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    
    MPI_Allreduce(&ploc,&ptot,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&txloc,&txtot,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    if(tw_ismaster()) {
      printf("Simulation started with %.0f particles and %.0f logical processors (%.3f part/LP)\n",
	     ntot0,ptot,ntot0/ptot);
      printf("Now, there are %.0f particles, of which %.0f are in transit (%.3f%%)\n",
	     ntot1+txtot,txtot,100.0*txtot/(ntot1+txtot));
      printf("Total run time: %.2lf sec\n",totalRunTime);
    }
  }

  /* Shutdown the ROSS simulation engine (and MPI) */ {
    tw_end();
  }

  return 0;
}
