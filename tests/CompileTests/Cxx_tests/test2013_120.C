// This is a test code that does not sucessfully reproduce a problem I notice in compiling test2013_app_03.C...

int x;

void foo()
{
   int x;
}


#if 0
void DotNode::write( FTextStream &t,
                   GraphType gt,
                   GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows,
                    bool reNumber
                   )
#else
void NEW_write()
#endif
{
   int x;
   int y;
   int z;
}
