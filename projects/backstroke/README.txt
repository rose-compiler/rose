Backstroke Design details

Goal: Support the automated generation of the reverse functions for optimistic discrete
      event simulation.

Data Structure Design:

    1) The forward function execution folows a dynamically defined path through the
       control flow of the application.
    2) The reverse computation follows this same path (but backwards) to a defined
       point that is known when the rollback is started.
    3) Event functions are rewritten to define forward functions and reverse functions
    4) The generated forward functions save information which the reverse functions will
       require if they are executed.
    5) The data structures associated with this problem are a central focus.
         a) There are two types of information:
             * Reverse queue, and
             * Commit queue.
         b) we will combine these to a single data structure.

The queues will be added to from the top,  The R queue will only be accessed from
the top (push and pop stack) and the C queue will be pushed and popped from the top
but also have elements removed from the bottom (as things are committed).
It will be helpful to have the same data structure for both.  And STL stack
might solve the R queue design, but it is hard to debug since only the top is available
and so a simpler list is easier to use in practice.  The list provides all the flexability
required for the C queue, since it can be operated upon at both ends.

The queues should be implemented using STL list data structures, and if we can make them
efficent later then we can consider that then.  An STL list data structure will provide
the required support for debugging (seeing the contents of the whole queue).

We will have many threads of execution, each will require their own set of R and C queues.
So perhaps a single data structure should package the R and C queues; and RC queue data
structure.


Our design should be independent of if we use it for rollback using state saving or
rollback using reverse computation.  
   1) Rollback using state saving
      This saves a pointer to a data structure representing save state 
      (saved at the top of the forward function).
      The saved state data-structure should be capable of representing
      arbitrary comp
   2) 


Testing:
   Generated forward and reverse functions will tested
using generated code that will test permutations of
function calls (automated testing).

*********************************************************************************

    Testing Harness for generated forward and reverse functions

We will define a simplified DES simulator that can test the execution of
combinations of forward and referse functions.  Our work will test a 
range over all permuations of forward and reverse generated code to 
test and evaluate the correctness and performance.

A component of the testing harness for our work will include a 
visualization of the rollback steps as either (state saving or 
reversible computation) to support debugging.  Mechanisms will
be used to report the performance in time and space.


*********************************************************************************

Optimization of generated code

   We will explore highest performance level of optimization possible for
the simple DES simulator (our testing harness).  We expect that defining
named shadow variables will define space for state storage as required
to support the highest level of performance.  A number of additional
ideas may improve the performance of our system using shadow variables:
   1) Co-locating variable on the stack and in global address space.
   2) Co-locating variable built on the heap.
Importantly optimizations may span multiple events.  Such 
optimizations could use saved variables at different points in the
chain of execution (e.g. even numbered steps and odd numbered steps)
to store ways of defining rollback more than one event at a time.
Such an approach could store a sequence of shadow variables as required
to rollback potentially log n steps and undo n forward events.

Slower approaches to saving rollback information will use a master
data structure that combines R and C queues and defines a simple
debugging interface to support our development work and experiment
with new ideas.

For forward functions that have side-effects on collections (e.g.
arrays, lists, etc.), an optimization may be to save the deltas
instead of the whole collection, up to a point.


**********************
Information about ROSS
**********************

ROSE will support a configure option to specify the location of 
ROSS from here we will use the header files in ROSS and the 
models in ROSS.

The SVN command is
   svn co https://pdes.svn.sourceforge.net/svnroot/pdes pdes

We are using ROSS version 5.0.
