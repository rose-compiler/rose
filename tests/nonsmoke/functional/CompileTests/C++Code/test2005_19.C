enum values { x,y };

int main()
   {
     values* selectionPtr = 0;

  // Call a vacuious (meaningless) destructor
     selectionPtr->~values();
   }
