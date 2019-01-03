// Lambda Capture of *this by Value as [=,*this]

class Work 
   {
     public:
          void do_something() const
             {
            // for ( int i = 0 ; i < N ; ++i )
               foreach( Parallel , 0 , N , [=,tmp=*this]( int i )
                  {
                 // A modestly long loop body where
                 // every reference to a member must be modified
                 // for qualification with 'tmp.'
                 // Any mistaken omissions will silently fail
                 // as references via 'this->'.
                  }
               );
             }
   };

