#ifndef BACKSTROKE_GENERATED

//#include <ross.h>
typedef int tw_lpid;
typedef int tw_bf;
/* Data structure to hold state of logical process (LP) */
typedef struct {
long long npart;
long long transit;}ctr_state;
/* Data structure to hold contents of a message/event */
typedef struct {
long long npart_send;}ctr_message;
typedef struct tw_event {
ctr_message dummy;}tw_event;
typedef struct tw_lp {
int rng;} tw_lp;
int g_tw_nlp = 1;
int TW_LOC = 0;

#else

/*******************************************************/

#if 0
/* Global variables */
static unsigned int total_no_lps = 0;
static const double mean = 5.0;
static const double lookahead = 1.0;
/* Event processing routine. Perform the relevant actions when event
   m happens to object s. */

#endif

using namespace Backstroke;

#include <cassert>

RunTimeLpStateStorage rtlpss;

void ctr_event_handler(ctr_state *s,tw_bf *bf,ctr_message *m,tw_lp *lp)
{
  assert(lp);
  Backstroke::RunTimeStateStorage* rts=rtlpss.getLpStateStorageAutoAllocated((void*)lp);
  assert(rts);
  rts->initializeForwardEvent();
  tw_stime eventSimTime=tw_now(lp);
  rts->setEventSimTime(eventSimTime);
  rts->commitEventsLessThanSimTime(lp->pe->GVT);
  /* We received the message m. Update local particle count,
     and number of particles in transit by number of received particles. */
  {
    (*rts->avpush(&(s -> npart))) += m -> npart_send;
    (*rts->avpush(&(s -> transit))) -= m -> npart_send;
  }
  /* Generate a new event to send some number of particles to
     another process (LP) */
  {
    tw_lpid destination;
    double event_time;
    long long nsend;
    ctr_message *msg;
    tw_event *evt;
    /* Let destination be a random other LP */
    destination = tw_rand_integer(lp -> rng,0,total_no_lps - 1);
    if (destination < 0 || destination >= g_tw_nlp * tw_nnodes()) {
      tw_error(TW_LOC,"bad dest");
    }
    /* Pick a random number of particles to send out */
    nsend = (tw_rand_integer(lp -> rng,0,(s -> npart + 1) / 2));
    /* Draw time at which sent out particles will arrive from exponential distribution */
    event_time = (tw_rand_exponential(lp -> rng,mean)) + lookahead;
    /* Create a new event, fill in how many particles to send,
       and send it*/
    {
      evt = (tw_event_new(destination,event_time,lp));
      msg = (ctr_message*) (tw_event_data(evt)); // MS: added explicit cast
#if 0
      msg->npart_send=nsend; // note: indirection
#else
      (*rts->avpush(&(msg->npart_send)))=nsend; // note: indirection
#endif

      tw_event_send(evt);
    }
    /* Update local particle count and number of particle sin transit */
    {
      (*rts->avpush(&(s -> npart))) -= nsend;
      (*rts->avpush(&(s -> transit))) += nsend;
    }
  }
  rts->finalizeForwardEvent();
}
/* Reverse event processing routine. Should perform the inverse of ctr_event_handler() */

void ctr_event_handler_rc(ctr_state *s,tw_bf *bf,ctr_message *m,tw_lp *lp)
{
  /* We used three random numbers in forward event processing.
     Roll the random number generator back threee times */ {
    tw_rand_reverse_unif(lp -> rng);
    tw_rand_reverse_unif(lp -> rng);
    tw_rand_reverse_unif(lp -> rng);
  }
  /* call backstroke-reverse function now */ {
    Backstroke::RunTimeStateStorage* rts=rtlpss.getLpStateStorageAutoAllocated((void*)lp);
    rts->reverseEvent();
  }
}

#endif
