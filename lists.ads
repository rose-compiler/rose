--::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
--LISTS.SPC
--::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

generic
    type Itemtype is private;  --| This is the data being manipulated.

    with function Equal (X, Y : in Itemtype) return Boolean is "=";
    --| This allows the user to define
    --| equality on ItemType.  For instance
    --| if ItemType is an abstract type
    --| then equality is defined in terms of
    --| the abstract type.  If this function
    --| is not provided equality defaults to
    --| =.
package Lists is

--| This package provides singly linked lists with elements of type
--| ItemType, where ItemType is specified by a generic parameter.

--| Overview
--| When this package is instantiated, it provides a linked list type for
--| lists of objects of type ItemType, which can be any desired type.  A
--| complete set of operations for manipulation, and releasing
--| those lists is also provided.  For instance, to make lists of strings,
--| all that is necessary is:
--|
--| type StringType is string(1..10);
--|
--| package Str_List is new Lists(StringType); use Str_List;
--|
--|    L:List;
--|    S:StringType;
--|
--| Then to add a string S, to the list L, all that is necessary is
--|
--|    L := Create;
--|    Attach(S,L);
--|
--|
--| This package provides basic list operations.
--|
--| Attach          append an object to an object, an object to a list,
--|                 or a list to an object, or a list to a list.

--| Copy            copy a list using := on elements
--| CopyDeep        copy a list by copying the elements using a copy
--|                 operation provided by the user
--| Create          Creates an empty list
--| DeleteHead      removes the head of a list
--| DeleteHeadDeep  removes the head of a list by handing to user first
--| DeleteItem      delete the first occurrence of an element from a list
--| DeleteItemDeep  delete first occurrence in list by handing to user first
--| DeleteItems     delete all occurrences of an element from a list
--| DeleteItemsDeep delete all occurrences in list by handing to user first
--| Destroy         remove a list
--| DestroyDeep     destroy a list as well as the elements in that list
--| Equal           are two lists equal
--| FirstValue      get the information from the first element of a list
--| Forward         advances an iterator
--| IsInList        determines whether a given element is in a given list
--| IsEmpty         returns true if the list is empty
--| LastValue       return the last value of a list
--| Length          Returns the length of a list
--| MakeList        this takes a single element and returns a list
--| MakeListIter    prepares for an iteration over a list
--| More            are there any more items in the list
--| Next            get the next item in a list
--| ModifyAt        passes item at iter to be modified by caller
--| DeleteAt        passes item at iter for possible deletion to caller
--| ReplaceHead     replace the information at the head of the list
--| ReplaceTail     replace the tail of a list with a new list
--| Tail            get the tail of a list
--| CellValue       this takes an iterator and returns the value of the element
--|                 whose position the iterator holds
--|

--| N/A: Effects, Requires, Modifies, and Raises.

--| Notes
--| Programmer Buddy Altus

--|                           Types
--|                           -----

    type List is private;
    type Listiter is private;


--|                           Exceptions
--|                           ----------

    Circularlist : exception;     --| Raised if an attemp is made to
				  --| create a circular list.  This
				  --| results when a list is attempted
				  --| to be attached to itself.

    Emptylist : exception;     --| Raised if an attemp is made to
			       --| manipulate an empty list.

    Itemnotpresent : exception;     --| Raised if an attempt is made to
				    --| remove an element from a list in
				    --| which it does not exist.

    Nomore : exception;     --| Raised if an attempt is made to
			    --| get the next element from a list
			    --| after iteration is complete.

    Listitermismatch : exception;   --| Raised when deleting an element using
				    --| the Iter if the List passed in does
				    --| not match the list for which the Iter
				    --| was created.



--|                           Operations
--|                           ----------

----------------------------------------------------------------------------

    procedure Attach (                  --| appends List2 to List1
		      List1 : in out List;  --| The list being appended to.
		      List2 : in List   --| The list being appended.
		      );

--| Raises
--| CircularList

--| Effects
--| Appends List1 to List2.  This makes the next field of the last element
--| of List1 refer to List2.  This can possibly change the value of List1
--| if List1 is an empty list.  This causes sharing of lists.  Thus if
--| user Destroys List1 then List2 will be a dangling reference.
--| This procedure raises CircularList if List1 equals List2.  If it is
--| necessary to Attach a list to itself first make a copy of the list and
--| attach the copy.

--| Modifies
--| Changes the next field of the last element in List1 to be List2.

-------------------------------------------------------------------------------

    function Attach
		(                 --| Creates a new list containing the two
				  --| Elements.
		 Element1 : in Itemtype;  --| This will be first element in list.
		 Element2 : in Itemtype   --| This will be second element in list.
		 ) return List;

--| Effects
--| This creates a list containing the two elements in the order
--| specified.

-------------------------------------------------------------------------------
    procedure Attach (                   --| List L is appended with Element.
		      L : in out List;      --| List being appended to.
		      Element : in
			 Itemtype   --| This will be last element in l    ist.
		      );

--| Effects
--| Appends Element onto the end of the list L.  If L is empty then this
--| may change the value of L.
--|
--| Modifies
--| This appends List L with Element by changing the next field in List.

--------------------------------------------------------------------------------
    procedure Attach (                   --| Makes Element first item in list L.
		      Element : in
		      Itemtype; --| This will be the first element in list.
		      L : in out List      --| The List which Element is being
					   --| prepended to.
		      );

--| Effects
--| This prepends list L with Element.
--|
--| Modifies
--| This modifies the list L.

--------------------------------------------------------------------------

    function Attach (                      --| attaches two lists
		     List1 : in List;           --| first list
		     List2 : in List            --| second list
		     ) return List;

--| Raises
--| CircularList

--| Effects
--| This returns a list which is List1 attached to List2.  If it is desired
--| to make List1 be the new attached list the following ada code should be
--| used.
--|
--| List1 := Attach (List1, List2);
--| This procedure raises CircularList if List1 equals List2.  If it is
--| necessary to Attach a list to itself first make a copy of the list and
--| attach the copy.

-------------------------------------------------------------------------

    function Attach (                   --| prepends an element onto a list
		     Element : in Itemtype;   --| element being prepended to list
		     L : in List        --| List which element is being added
					--| to
		     ) return List;

--| Effects
--| Returns a new list which is headed by Element and followed by L.

------------------------------------------------------------------------

    function Attach
		(                  --| Adds an element to the end of a list
		 L : in List;      --| The list which element is being added to.
		 Element : in Itemtype   --| The element being added to the end of
					 --| the list.
		 ) return List;

--| Effects
--| Returns a new list which is L followed by Element.

--------------------------------------------------------------------------

    function Copy (          --| returns a copy of list1
		   L : in List       --| list being copied
		   ) return List;

--| Effects
--| Returns a copy of L.

--------------------------------------------------------------------------

    generic
	with function Copy (I : in Itemtype) return Itemtype;


    function Copydeep (      --| returns a copy of list using a user supplied
			     --| copy function.  This is helpful if the type
			     --| of a list is an abstract data type.
		       L : in List --| List being copied.
		       ) return List;

--| Effects
--| This produces a new list whose elements have been duplicated using
--| the Copy function provided by the user.

------------------------------------------------------------------------------

    function Create           --| Returns an empty List

		return List;

------------------------------------------------------------------------------

    procedure Deletehead
		 (            --| Remove the head element from a list.
		  L : in out List         --| The list whose head is being removed.
		  );

--| RAISES
--| EmptyList
--|
--| EFFECTS
--| This will return the space occupied by the first element in the list
--| to the heap.  If sharing exists between lists this procedure
--| could leave a dangling reference.  If L is empty EmptyList will be
--| raised.

------------------------------------------------------------------------------

    generic

	with procedure Dispose (Item : in out Itemtype);

    procedure Deleteheaddeep
		 (            --| Remove the head element from a list.
		  L : in out List         --| The list whose head is being removed.
		  );

--| RAISES
--| EmptyList
--|
--| EFFECTS
--| This will return the space occupied by the first element in the list
--| to the heap.  It calls the user-instantiated Dispose first to allow
--| the instantiator to clean-up any dynamically allocated memory before
--| the list element is returned to the system.  If sharing exists between
--| lists this procedure could leave a dangling reference.  If L is empty
--| EmptyList will be raised.

------------------------------------------------------------------------------

    procedure Deleteitem
		 (           --| remove the first occurrence of Element
			     --| from L
		  L : in out List;     --| list element is being  removed from
		  Element : in Itemtype  --| element being removed
		  );

--| EFFECTS
--| Removes the first element of the list equal to Element.  If there is
--| not an element equal to Element than ItemNotPresent is raised.

--| MODIFIES
--| This operation is destructive, it returns the storage occupied by
--| the elements being deleted.

------------------------------------------------------------------------------

    generic
	with procedure Dispose (Item : in out Itemtype);
    procedure Deleteitemdeep
		 (           --| remove the first occurrence of Element
			     --| from L
		  L : in out List;     --| list element is being  removed from
		  Element : in Itemtype  --| element being removed
		  );

--| EFFECTS
--| Removes the first element of the list equal to Element.  If there is
--| not an element equal to Element or the list is empty then ItemNotPresent
--| is raised.  Calls Dispose so that user can perform any dynamic memory
--| clean-up before Element is removed from list.

--| MODIFIES
--| This operation is destructive, it returns the storage occupied by
--| the elements being deleted.

----------------------------------------------------------------------------

    function Deleteitem (            --| remove the first occurrence of Element
				     --| from L
			 L : in List;     --| list element is being  removed from
			 Element : in Itemtype  --| element being removed
			 ) return List;

--| EFFECTS
--| This returns the List L with the first occurrence of Element removed.

------------------------------------------------------------------------------

    function Deleteitems (          --| remove all occurrences of Element
				    --| from  L.
			  L : in List;     --| The List element is being removed from
			  Element : in Itemtype  --| element being removed
			  ) return List;

--| EFFECTS
--| This function returns a copy of the list L which has all elements which
--| have value Element removed.

-------------------------------------------------------------------------------

    procedure Deleteitems
		 (         --| remove all occurrences of Element
			   --| from  L.
		  L : in out List;     --| The List element is being removed from
		  Element : in Itemtype  --| element being removed
		  );

--| EFFECTS
--| This procedure removes all occurrences of Element from the List L.  This
--| is a destructive procedure.

------------------------------------------------------------------------------

    generic

	with procedure Dispose (Item : in out Itemtype);
    procedure Deleteitemsdeep
		 (           --| remove all occurrences of Element
			     --| from L
		  L : in out List;     --| list element is being  removed from
		  Element : in Itemtype  --| element being removed
		  );

--| EFFECTS
--| Removes all elements of the list equal to Element.  Calls
--| Dispose so that user can perform any dynamic memory clean-up before
--| each Element is removed from list.

--| MODIFIES
--| This operation is destructive, it returns the storage occupied by
--| the elements being deleted.

------------------------------------------------------------------------------

    procedure Destroy (           --| removes the list
		       L : in out List      --| the list being removed
		       );

--| Effects
--| This returns to the heap all the storage that a list occupies.  Keep in
--| mind if there exists sharing between lists then this operation can leave
--| dangling references.

------------------------------------------------------------------------------
    generic
	with procedure Dispose (I : in out Itemtype);

    procedure Destroydeep (  --| Destroy a list as well as all objects which
			     --| comprise an element of the list.
			   L : in out List);


--| OVERVIEW
--| This procedure is used to destroy a list and all the objects contained
--| in an element of the list.  For example if L is a list of lists
--| then destroy L does not destroy the lists which are elements of L.
--| DestroyDeep will now destroy L and all the objects in the elements of L.
--| The produce Dispose is a procedure which will destroy the objects which
--| comprise an element of a list.  For example if package  L was  a list
--| of lists then Dispose for L would be the Destroy of list type package L was
--| instantiated with.

--| REQUIRES
--| This procedure requires no sharing  between elements of lists.
--| For example if L_int is a list of integers and L_of_L_int is a list
--| of lists of integers and two elements of L_of_L_int have the same value
--| then doing a DestroyDeep will cause an access violation to be raised.
--| The best way to avoid this is not to have sharing between list elements
--| or use copy functions when adding to the list of lists.

------------------------------------------------------------------------------

    function Firstvalue (      --| returns the contents of the first record of the
			       --| list
			 L : in List       --| the list whose first element is being
					   --| returned

			 ) return Itemtype;

--| Raises
--| EmptyList
--|
--| Effects
--| This returns the Item in the first position in the list.  If the list
--| is empty EmptyList is raised.

-------------------------------------------------------------------------------

    procedure Forward (            --| Advances the iterator.
		       I : in out Listiter   --| The iterator.
		       );

--| Raises NoMore if Listiter is at the end of the list.
--|
--| OVERVIEW
--| This procedure can be used in conjunction with Cell to iterate over a list.
--| This is in addition to Next.  Instead of writing
--|
--|  I :ListIter;
--|  L :List;
--|  V :List_Element_Type;
--|
--|  I := MakeListIter(L);
--|  while More(I) loop
--|      Next (I, V);
--|      Print (V);
--|  end loop;
--|
--| One can write
--| I := MakeListIter(L);
--| while More (I) loop
--|     Print (CellValue (I));
--|     Forward (I);
--| end loop;

-------------------------------------------------------------------------------

    function Isempty (            --| Checks if a list is empty.
		      L : in List      --| List being checked.
		      ) return Boolean;

--------------------------------------------------------------------------

    function Isinlist (                 --| Checks if element is an element of
					--| list.
		       L : in List;     --| list being scanned for element
		       Element : in Itemtype  --| element being searched for
		       ) return Boolean;

--| Effects
--| Walks down the list L looking for an element whose value is Element.

------------------------------------------------------------------------------

    function Lastvalue (       --| Returns the contents of the last record of
			       --| the list.
			L : in List       --| The list whose first element is being
					  --| returned.
			) return Itemtype;

--| Raises
--| EmptyList
--|
--| Effects
--| Returns the last element in a list.  If the list is empty EmptyList is
--| raised.


------------------------------------------------------------------------------

    function Length (         --| count the number of elements on a list
		     L : in List      --| list whose length is being computed
		     ) return Integer;

------------------------------------------------------------------------------

    function Makelist (   --| This takes in an element and returns a List.
		       E : in Itemtype) return List;

------------------------------------------------------------------------------

    function Makelistiter (          --| Sets a variable to point to  the head
				     --| of the list.  This will be used to
				     --| prepare for iteration over a list.
			   L : in List             --| The list being iterated over.
			   ) return Listiter;


--| This prepares a user for iteration operation over a list.  The iterater is
--| an operation which returns successive elements of the list on successive
--| calls to the iterator.  There needs to be a mechanism which marks the
--| position in the list, so on successive calls to the Next operation the
--| next item in the list can be returned.  This is the function of the
--| MakeListIter and the type ListIter.  MakeIter just sets the Iter to the
--| the beginning  of the list. On subsequent calls to Next the Iter
--| is updated with each call.

-----------------------------------------------------------------------------

    function More (           --| Returns true if there are more elements in
			      --| the and false if there aren't any more
			      --| the in the list.
		   Place : in Listiter  --| List being checked for elements.
		   ) return Boolean;

------------------------------------------------------------------------------

    procedure Next (                 --| This is the iterator operation.  Given
				     --| a ListIter in the list it returns the
				     --| current item and updates the ListIter.
				     --| If ListIter is at the end of the list,
				     --| More returns false otherwise it
				     --| returns true.
		    Place : in out
		    Listiter;  --| The Iter which marks the position in
			       --| the list.
		    Info : out Itemtype   --| The element being returned.

		    );

--| Raises NoMore if Listiter is at the end of the list.
--|
--| The iterators subprograms MakeListIter, More, and Next should be used
--| in the following way:
--|
--|         L:        List;
--|         Place:    ListIter;
--|         Info:     SomeType;
--|
--|
--|         Place := MakeListIter(L);
--|
--|         while ( More(Place) ) loop
--|               Next(Place, Info);
--|               process each element of list L;
--|         end loop;

------------------------------------------------------------------------------

    generic

	with procedure Modifyitem (Item : in out Itemtype);

    procedure Modifyat
		 (             --| This is the iterator operation.  Given
			       --| a ListIter in the list it calls
			       --| ModifyItem for the current item and
			       --| does not update the ListIter.
			       --| To get to the next element call Forward.

		  Place : in Listiter  --| The Iter which marks list position.
		  );

--| Raises NoMore if Listiter is at the end of the list.
--|
--| The iterators subprograms MakeListIter, More, and ModifyAt should be used
--| in the following way:
--|
--|         L:        List;
--|         Place:    ListIter;
--|
--|         procedure ModifyTheItem( Item : in out SomeType ) is
--|         begin
--|             ... do some work with Item ...
--|         end ModifyTheItem;
--|
--|         procedure MyModifyItem is
--|            new ListsInstance.ModifyAt( ModifyTheItem );
--|
--|
--|         Place := MakeListIter(L);
--|
--|         while ( More( Place )) loop
--|               MyModifyItem( Place );
--|               Forward( Place );
--|         end loop;

------------------------------------------------------------------------------

    generic

	with procedure Deleteitem
			  (Item : in out Itemtype; Delete : out Boolean);

    procedure Deleteat
		 (             --| This is the iterator operation.  Given
			       --| a ListIter in the list it calls
			       --| DeleteItem for the current item and
			       --| updates the ListIter.

		  L : in out List;     --| The list to which the iter belongs
		  Place : in out Listiter  --| The Iter which marks list position.
		  );

--| Raises NoMore if Listiter is at the end of the list.
--|        ListIterMismatch if L does not match the original list for
--|            which Place was created.
--|
--| The Item is only deleted if Delete is set to true.  Place is always moved
--| to the next item in the list, so there is no need for Forward.
--|
--| The iterators subprograms MakeListIter, More, and DeleteAt should be used
--| in the following way:
--|
--|         L:        List;
--|         Place:    ListIter;
--|
--|         procedure DeleteTheItem     (
--|                     Item     : in out SomeType;
--|                     Delete   : out Boolean ) is
--|         begin
--|             ... do some work with Item ...
--|             set Delete to true or false
--|         end DeleteTheItem;
--|
--|         procedure MyDeleteItem is
--|            new ListsInstance.DeleteAt( DeleteTheItem );
--|
--|
--|         Place := MakeListIter(L);
--|
--|         while ( More( Place )) loop
--|               MyDeleteItem( Place );
--|         end loop;

----------------------------------------------------------------------------

    procedure Replacehead (     --| Replace the Item at the head of the list
				--| with the parameter Item.
			   L : in out List;    --| The list being modified.
			   Info : in Itemtype --| The information being entered.
			   );
--| Raises
--| EmptyList

--| Effects
--| Replaces the information in the first element in the list.  Raises
--| EmptyList if the list is empty.

------------------------------------------------------------------------------

    procedure Replacetail (           --| Replace the Tail of a list
				      --| with a new list.
			   L : in out List;  --| List whose Tail is replaced.
			   Newtail : in List   --| The list which will become the
					       --| tail of Oldlist.
			   );
--| Raises
--| EmptyList
--|
--| Effects
--| Replaces the tail of a list with a new list.  If the list whose tail
--| is being replaced is null EmptyList is raised.

-------------------------------------------------------------------------------

    function Tail (           --| returns the tail of a list L
		   L : in List      --| the list whose tail is being returned
		   ) return List;

--| Raises
--| EmptyList
--|
--| Effects
--| Returns a list which is the tail of the list L.  Raises EmptyList if
--| L is empty.  If L only has one element then Tail returns the Empty
--| list.

------------------------------------------------------------------------------

    function Cellvalue
		(    --| Return the value of the element where the iterator is
		     --| positioned.
		 I : in Listiter) return Itemtype;

--| Raises NoMore if Listiter is at the end of the list.
--|
--| OVERVIEW
--| This returns the value of the element at the position of the iterator.
--| This is used in conjunction with Forward.

--------------------------------------------------------------------------


    function Equal (            --| compares list1 and list2 for equality
		    List1 : in List;   --| first list
		    List2 : in List    --| second list
		    ) return Boolean;

--| Effects
--| Returns true if for all elements of List1 the corresponding element
--| of List2 has the same value.  This function uses the Equal operation
--| provided by the user.  If one is not provided then = is used.

------------------------------------------------------------------------------
private
    type Cell;

    type List is access Cell;      --| pointer added by this package
				   --| in order to make a list


    type Cell is                   --| Cell for the lists being created
	record
	    Info : Itemtype;
	    Next : List;
	end record;


    type Listiter is
	record
	    Currentiter : List := null;  -- the one we operate on
	    Previousiter : List := null;  -- need this to delete
	    Origlisthead : List := null;  -- the head of the list for which the
					  -- iter was created.  may change if
					  -- DeleteAt is called on the head
					  -- of the list.
	end record;

end Lists;
