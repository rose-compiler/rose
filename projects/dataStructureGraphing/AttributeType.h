// ---------
// AttributeType3.h
// ---------

#ifndef AttributeType3_h
#define AttributeType3_h
#include <sstream>
#include <string>
#include <iostream>  // ostringstream
#include <algorithm> // swap
#include <cassert>   // assert

// using namespace std;

//Andreas Sæbjørnsen 14/09/04 
//
//     class AtributeType
// A class constructed to be a flexible AttributeType in a complex traversal generating 
// C/C++ code. This is supposed to be a tool to assist while constructing the string
// which is to be inserted by the ASTRewrite system. With this it is easy to introduce
// the concept of scopes without yet haveing valid code or a subtree of the AST representing
// the scopes which is to be used.


class AttributeType {
    public:
      typedef enum 
             {
               unknownPositionInScope  = 0 /*!< unknown used as a default value for error checking */ ,
               PreamblePositionInScope = 1 /*!< Source code to be placed at the top of a specified scope */ ,
               TopOfCurrentScope       = 2 /*!< Top of scope (current location must be a scope)  */ ,
               AfterCurrentPosition    = 3 /*!< After   */ ,
               BottomOfCurrentScope    = 4 /*!< Bottom of scope (current location must be a scope)  */ ,
               GlobalScope             = 5 /*!< Source code to be placed in global scope */,
               LAST_PLACEMENT_TAG
             } PlacementPositionEnum;



        typedef std::ostringstream         value_type;

        typedef std::ostringstream*        pointer;
        typedef const std::ostringstream*  const_pointer;

        typedef std::ostringstream&        reference;
        typedef const std::ostringstream&  const_reference;

    private:
        struct container {
            int  numberBeginScopes;
            int  numberEndScopes;
      
            //create a queue which can contain all the cases
            //in the PlacementPositionEnum. The goal is to make
            //it possible to simulate an increase of buffer size in
            //value_type so that you can put a string into the
            //middle of it without overwriting it
            pointer queue[2*LAST_PLACEMENT_TAG];
            int     posTracker[LAST_PLACEMENT_TAG];
            //variables to define where the different positions are


            pointer collapseQueue(){
                   pointer out = queue[0];
                   assert( out !=  NULL);
                   for(int i = 1; i <=posTracker[LAST_PLACEMENT_TAG]; i++){
                      assert(queue[i]!=NULL);
                      (*out) << queue[i]->str();
                      if(i != posTracker[LAST_PLACEMENT_TAG]){
                        //delete queue[i];
                      }else{
                        queue[1] = queue[i];
                        queue[1]->flush();
                        posTracker[i] = 1;
                      }
                    }
 
                   for(int i = 1; i<LAST_PLACEMENT_TAG;i++){
                       posTracker[i]=-1;
                   }
                    
                   return out;
            }

            // The positions in the array is as follows
            //    [oldest set buffer, oldest-1, ...., current buffer]  
            // When a new position is set the old one is added to
            // the [buffer to delete+1] and then deleted.
            void newPos(PlacementPositionEnum pos, pointer ptr=NULL){
                 assert(pos<LAST_PLACEMENT_TAG);
                 assert(posTracker[LAST_PLACEMENT_TAG]+1<2*LAST_PLACEMENT_TAG);
                 int position = posTracker[pos];
                 if(position >= 0){
                    assert(queue[position+1]!=NULL);
                    assert(queue[position]  !=NULL);
                    (*queue[position]) << queue[position+1]->str();
                   
                    //flush the previous position and reuse it
                    //if applicable
                    pointer tmp;
                    if(ptr==NULL){
                       tmp = queue[position+1];
                       tmp->flush();
                    }else{
                       delete queue[position+1];
                       tmp = new value_type();
                    }

                    //restack the queue according to the structure
                    for(int i = unknownPositionInScope; i <LAST_PLACEMENT_TAG; i++)
                       if(posTracker[i]>=position)
                         posTracker[i]--;
                    for(int i = position+2; position < LAST_PLACEMENT_TAG; i++){
                          assert(queue[i]!=NULL);
                          queue[i-1]=queue[i];
                    }
                    posTracker[pos] = posTracker[LAST_PLACEMENT_TAG]-1;
                    queue[posTracker[LAST_PLACEMENT_TAG]-1] = queue[posTracker[LAST_PLACEMENT_TAG]];
                    queue[posTracker[LAST_PLACEMENT_TAG]] = tmp; 
                 }else{ 
                    //Move the last element in the queue one place up and insert a
                    //new value_type in the previous position
                    int lastPosition = posTracker[LAST_PLACEMENT_TAG];
                    assert(queue[lastPosition] != NULL );
      
                    if(ptr==NULL)
                       queue[lastPosition+1]   = new value_type();
                    else
                       queue[lastPosition+1]   = ptr;
                    posTracker[pos] = posTracker[LAST_PLACEMENT_TAG];
                    posTracker[LAST_PLACEMENT_TAG]++;

                 }
            }

            container (pointer scopeInit) :
                    numberBeginScopes (0),
                    numberEndScopes (0){
                assert(scopeInit);
                //The global scope is allways set to the end of the scope
               
                for(int i = 0; i < 2*LAST_PLACEMENT_TAG; i++){
                   queue[i]=NULL;
                   if(i<LAST_PLACEMENT_TAG)
                     posTracker[i] = -1;
                }
                //Create a pointer to print the current scope
                queue[0] = new value_type();
                posTracker[LAST_PLACEMENT_TAG] = 0;
                newPos(unknownPositionInScope, scopeInit);

               
                }

            ~container () {
                //delete scopeInfo;
            };
         };

        container* p;

    public:
        pointer get () {
            return p->collapseQueue();}

        const_pointer get () const {
            return p->collapseQueue();}

        AttributeType (pointer q) :
                p (new container(q)) {
            assert(p);}

        AttributeType (const AttributeType& that) :
                p (that.p) {
            assert(p);
        }

        ~AttributeType () {
            if (p->numberBeginScopes==p->numberEndScopes)
                delete p;
            else{
                std::cout << "The number of begun scopes is '" << p->numberBeginScopes
                     << "' and the number of ended scopes is '" << p->numberEndScopes
                     << ". \n That is a severe error and will result in compile error.\n";
                exit(1); 
            }
        }

        AttributeType& operator = (const AttributeType& x) {
            AttributeType that(x);
            swap(that);
            return *this;
        }
       
        //Insert a typename T into the end of the stream 
        template <typename T>
        value_type& operator << (const T& x) {
            assert(p);
                        
            int posInQueue = p->posTracker[LAST_PLACEMENT_TAG];
            pointer out = p->queue[posInQueue];
            assert(posInQueue>=0);
            assert(out!=NULL);
        
            (*out) << x; 

            return *out;
         } 


        //Operator () makes it possible to define at which position in the stream the typename T should
        //be inserted. The positions must be set before that happens. Only current->afterCurrent is set
        //by default.
        template <typename T>
        value_type& operator () (const T& x, PlacementPositionEnum pos) {
            assert(p);
            int posInQueue = p->posTracker[pos];
            pointer out = p->queue[posInQueue];
            assert(posInQueue>=0);
            assert(out!=NULL);
       
            (*out) << x;

            return *p->queue[p->posTracker[LAST_PLACEMENT_TAG]];
        } 
     
        //Set the position of the PlacementPositionEnum case 'pos' to the point in the stream which
        //currently is the end of the stream.
        void set(PlacementPositionEnum pos){
             assert(p);
             p->newPos(pos);
         } 

       void swap (AttributeType& that) {
            std::swap(p, that.p);
        }
};

#endif // AttributeType3_h
