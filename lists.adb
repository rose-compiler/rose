-- 2004/01/20 CAR GNAT warning - commented out unused declarations

--with ada.text_io;    use ada.text_io;

with Unchecked_Deallocation;

package body Lists is

    procedure Free is new Unchecked_Deallocation (Cell, List);

--------------------------------------------------------------------------

    function Last (L : in List) return List is

        Place_In_L : List;
        Temp_Place_In_L : List;

        --|  Link down the list L and return the pointer to the last element
        --| of L.  If L is null raise the EmptyList exception.

    begin
        if L = null then
            raise Emptylist;
        else

            --|  Link down L saving the pointer to the previous element in
            --|  Temp_Place_In_L.  After the last iteration Temp_Place_In_L
            --|  points to the last element in the list.

            Place_In_L := L;
            while Place_In_L /= null loop
                Temp_Place_In_L := Place_In_L;
                Place_In_L := Place_In_L.Next;
            end loop;
            return Temp_Place_In_L;
        end if;
    end Last;


--------------------------------------------------------------------------

    procedure Attach (List1 : in out List; List2 : in List) is
        Endoflist1 : List;

        --| Attach List2 to List1.
        --| If List1 is null return List2
        --| If List1 equals List2 then raise CircularList
        --| Otherwise get the pointer to the last element of List1 and change
        --| its Next field to be List2.

    begin
        if List1 = null then
            List1 := List2;
            return;
        elsif List1 = List2 then
            raise Circularlist;
        else
            Endoflist1 := Last (List1);
            Endoflist1.Next := List2;
        end if;
    end Attach;

--------------------------------------------------------------------------

    procedure Attach (L : in out List; Element : in Itemtype) is

        Newend : List;

        --| Create a list containing Element and attach it to the end of L

    begin
        Newend := new Cell'(Info => Element, Next => null);
        Attach (L, Newend);
    end Attach;

--------------------------------------------------------------------------

    function Attach
                (Element1 : in Itemtype; Element2 : in Itemtype) return List is
        Newlist : List;

        --| Create a new list containing the information in Element1 and
        --| attach Element2 to that list.

    begin
        Newlist := new Cell'(Info => Element1, Next => null);
        Attach (Newlist, Element2);
        return Newlist;
    end Attach;

--------------------------------------------------------------------------

    procedure Attach (Element : in Itemtype; L : in out List) is

        --|  Create a new cell whose information is Element and whose Next
        --|  field is the list L.  This prepends Element to the List L.

    begin
        L := new Cell'(Info => Element, Next => L);
    end Attach;

--------------------------------------------------------------------------

    function Attach (List1 : in List; List2 : in List) return List is

        Last_Of_List1 : List;

    begin
        if List1 = null then
            return List2;
        elsif List1 = List2 then
            raise Circularlist;
        else
            Last_Of_List1 := Last (List1);
            Last_Of_List1.Next := List2;
            return List1;
        end if;
    end Attach;

-------------------------------------------------------------------------

    function Attach (L : in List; Element : in Itemtype) return List is

        Newend : List;
        Last_Of_L : List;

        --| Create a list called NewEnd and attach it to the end of L.
        --| If L is null return NewEnd
        --| Otherwise get the last element in L and make its Next field
        --| NewEnd.

    begin
        Newend := new Cell'(Info => Element, Next => null);
        if L = null then
            return Newend;
        else
            Last_Of_L := Last (L);
            Last_Of_L.Next := Newend;
            return L;
        end if;
    end Attach;

--------------------------------------------------------------------------

    function Attach (Element : in Itemtype; L : in List) return List is

    begin
        return (new Cell'(Info => Element, Next => L));
    end Attach;

---------------------------------------------------------------------------


    function Copy (L : in List) return List is

        --| If L is null return null
        --| Otherwise recursively copy the list by first copying the information
        --| at the head of the list and then making the Next field point to
        --| a copy of the tail of the list.

    begin
        if L = null then
            return null;
        else
            return new Cell'(Info => L.Info, Next => Copy (L.Next));
        end if;
    end Copy;


--------------------------------------------------------------------------

    function Copydeep (L : in List) return List is

        --|  If L is null then return null.
        --|  Otherwise copy the first element of the list into the head of the
        --|  new list and copy the tail of the list recursively using CopyDeep.

    begin
        if L = null then
            return null;
        else
            return new Cell'(Info => Copy (L.Info), Next => Copydeep (L.Next));
        end if;
    end Copydeep;

--------------------------------------------------------------------------

    function Create return List is

        --| Return the empty list.

    begin
        return null;
    end Create;

--------------------------------------------------------------------------
    procedure Deletehead (L : in out List) is

        Templist : List;

        --| Remove the element of the head of the list and return it to the heap.
        --| If L is null EmptyList.
        --| Otherwise save the Next field of the first element, remove the first
        --| element and then assign to L the Next field of the first element.

    begin
        if L = null then
            raise Emptylist;
        else
            Templist := L.Next;
            Free (L);
            L := Templist;
        end if;
    end Deletehead;

--------------------------------------------------------------------------

    -- generic
--
--         with procedure Dispose( Item : in out Itemtype );

    procedure Deleteheaddeep (L : in out List) is

        Templist : List;

    begin
        if L = null then
            raise Emptylist;
        else
            Dispose (L.Info);
            Templist := L.Next;
            Free (L);
            L := Templist;
        end if;

    end Deleteheaddeep;

--------------------------------------------------------------------------

    function Deleteitem (            --| remove the first occurrence of Element
                                     --| from L
                         L : in List;     --| list element is being  removed from
                         Element : in Itemtype  --| element being removed
                         ) return List is
        I : List;
        Result : List;
        Found : Boolean := False;
    begin
        --| ALGORITHM
        --| Attach all elements of L to Result except the first element in L
        --| whose value is Element.  If the current element pointed to by I
        --| is not equal to element or the element being skipped was found
        --| then attach the current element to Result.

        I := L;
        while (I /= null) loop
            if (not Equal (I.Info, Element)) or (Found) then
                Attach (Result, I.Info);
            else
                Found := True;
            end if;
            I := I.Next;
        end loop;
        return Result;
    end Deleteitem;

------------------------------------------------------------------------------

    function Deleteitems (          --| remove all occurrences of Element
                                    --| from  L.
                          L : in List;     --| The List element is being removed from
                          Element : in Itemtype  --| element being removed
                          ) return List is
        I : List;
        Result : List;
    begin
        --| ALGORITHM
        --| Walk over the list L and if the current element does not equal
        --| Element then attach it to the list to be returned.

        I := L;
        while I /= null loop
            if not Equal (I.Info, Element) then
                Attach (Result, I.Info);
            end if;
            I := I.Next;
        end loop;
        return Result;
    end Deleteitems;

-------------------------------------------------------------------------------

    procedure Deleteitem (L : in out List; Element : in Itemtype) is

        --| Remove the first element in the list with the value Element.
        --| If the first element of the list is equal to element then
        --| remove it.  Otherwise, recurse on the tail of the list.

    begin
        if Equal (L.Info, Element) then
            Deletehead (L);
        else
            Deleteitem (L.Next, Element);
        end if;
    end Deleteitem;

-------------------------------------------------------------------------------

    -- generic
--
--         with procedure Dispose( Item : in out Itemtype );

    procedure Deleteitemdeep (L : in out List; Element : in Itemtype) is

        --| Remove the first element in the list with the value Element.
        --| If the first element of the list is equal to element then
        --| remove it.  Otherwise, recurse on the tail of the list.

        procedure Localdeleteheaddeep is new Deleteheaddeep (Dispose);

    begin
        if (L = null) then
            raise Itemnotpresent;
        end if;

        if Equal (L.Info, Element) then
            Localdeleteheaddeep (L);
        else
            Deleteitemdeep (L.Next, Element);
        end if;

    end Deleteitemdeep;

--------------------------------------------------------------------------

    procedure Deleteitems (L : in out List; Element : in Itemtype) is

        Place_In_L : List;     --| Current place in L.
        Last_Place_In_L : List;     --| Last place in L.
        Temp_Place_In_L : List;     --| Holds a place in L to be removed.

        --| Walk over the list removing all elements with the value Element.

    begin
        Place_In_L := L;
        Last_Place_In_L := null;
        while (Place_In_L /= null) loop
            --| Found an element equal to Element
            if Equal (Place_In_L.Info, Element) then
                --| If Last_Place_In_L is null then we are at first element
                --| in L.
                if Last_Place_In_L = null then
                    Temp_Place_In_L := Place_In_L;
                    L := Place_In_L.Next;
                else
                    Temp_Place_In_L := Place_In_L;

                    --| Relink the list Last's Next gets Place's Next

                    Last_Place_In_L.Next := Place_In_L.Next;
                end if;

                --| Move Place_In_L to the next position in the list.
                --| Free the element.
                --| Do not update the last element in the list it remains the
                --| same.

                Place_In_L := Place_In_L.Next;
                Free (Temp_Place_In_L);
            else
                --| Update the last place in L and the place in L.

                Last_Place_In_L := Place_In_L;
                Place_In_L := Place_In_L.Next;
            end if;
        end loop;

        --| If we have not found an element raise an exception.

    end Deleteitems;

--------------------------------------------------------------------------

    -- generic
--
--         with procedure Dispose( Item : in out Itemtype );

    procedure Deleteitemsdeep (L : in out List; Element : in Itemtype) is

        Place_In_L : List;     --| Current place in L.
        Last_Place_In_L : List;     --| Last place in L.
        Temp_Place_In_L : List;     --| Holds a place in L to be removed.

        --| Walk over the list removing all elements with the value Element.

    begin
        Place_In_L := L;
        Last_Place_In_L := null;
        while (Place_In_L /= null) loop
--
-- See if this element is equal to Element sought.
--
            if Equal (Place_In_L.Info, Element) then
                Temp_Place_In_L := Place_In_L;
--
--  If Last_Place_In_L is null then we are at first element in L.
--
                if Last_Place_In_L = null then
                    L := Place_In_L.Next;
                else
                    Last_Place_In_L.Next := Place_In_L.Next;
                end if;
--
-- Move Place_In_L to the next position in the list.  Free the element.
-- Do not update the last element in the list, it remains the same.
--
                Place_In_L := Place_In_L.Next;
                Dispose (Temp_Place_In_L.Info);   -- let user at the element
                Free (Temp_Place_In_L);
            else
--
-- Update the last place in L and the place in L.
--
                Last_Place_In_L := Place_In_L;
                Place_In_L := Place_In_L.Next;
            end if;
        end loop;

    end Deleteitemsdeep;

------------------------------------------------------------------------------

    procedure Destroy (L : in out List) is

        Place_In_L : List;
        Holdplace : List;

        --| Walk down the list removing all the elements and set the list to
        --| the empty list.

    begin
        Place_In_L := L;
        while Place_In_L /= null loop
            Holdplace := Place_In_L;
            Place_In_L := Place_In_L.Next;
            Free (Holdplace);
        end loop;
        L := null;
    end Destroy;

--------------------------------------------------------------------------

    procedure Destroydeep (L : in out List) is

        Place_In_L : List;
        Holdplace : List;

        --| Walk down the list removing all the elements and set the list to
        --| the empty list.

    begin
        Place_In_L := L;
        while Place_In_L /= null loop
            Holdplace := Place_In_L;
            Place_In_L := Place_In_L.Next;
            Dispose (Holdplace.Info);
            Free (Holdplace);
        end loop;
        L := null;
    end Destroydeep;

--------------------------------------------------------------------------

    function Firstvalue (L : in List) return Itemtype is

        --| Return the first value in the list.

    begin
        if L = null then
            raise Emptylist;
        else
            return (L.Info);
        end if;
    end Firstvalue;

--------------------------------------------------------------------------

    procedure Forward (I : in out Listiter) is

        --| Return the pointer to the next member of the list.

    begin
        if (not More (I)) then
            raise Nomore;
        else
            I.Previousiter := I.Currentiter;
            I.Currentiter := I.Currentiter.Next;
        end if;

    end Forward;

--------------------------------------------------------------------------

    function Isinlist (L : in List; Element : in Itemtype) return Boolean is

        Place_In_L : List;

        --| Check if Element is in L.  If it is return true otherwise return false.

    begin
        Place_In_L := L;
        while Place_In_L /= null loop
            if Equal (Place_In_L.Info, Element) then
                return True;
            end if;
            Place_In_L := Place_In_L.Next;
        end loop;
        return False;
    end Isinlist;

--------------------------------------------------------------------------

    function Isempty (L : in List) return Boolean is

        --| Is the list L empty.

    begin
        return (L = null);
    end Isempty;

--------------------------------------------------------------------------

    function Lastvalue (L : in List) return Itemtype is

        Lastelement : List;

        --| Return the value of the last element of the list. Get the pointer
        --| to the last element of L and then return its information.

    begin
        Lastelement := Last (L);
        return Lastelement.Info;
    end Lastvalue;

--------------------------------------------------------------------------

    function Length (L : in List) return Integer is

        --| Recursively compute the length of L.  The length of a list is
        --| 0 if it is null or  1 + the length of the tail.

    begin
        if L = null then
            return (0);
        else
            return (1 + Length (Tail (L)));
        end if;
    end Length;

--------------------------------------------------------------------------

    function Makelist (E : in Itemtype) return List is

    begin
        return new Cell'(Info => E, Next => null);
    end Makelist;

--------------------------------------------------------------------------
    function Makelistiter (L : in List) return Listiter is

        --| Start an iteration operation on the list L.

    begin
        return (Currentiter => L, Previousiter => null, Origlisthead => L);

    end Makelistiter;

--------------------------------------------------------------------------

    function More (Place : in Listiter) return Boolean is

        --| This is a test to see whether an iteration is complete.

    begin
        return Place.Currentiter /= null;

    end More;

--------------------------------------------------------------------------

    procedure Next (Place : in out Listiter; Info : out Itemtype) is

        --| This procedure gets the information at the current place in the List
        --| and moves the ListIter to the next postion in the list.
        --| If we are at the end of a list then exception NoMore is raised.

    begin
        if (not More (Place)) then
            raise Nomore;
        else
            Info := Place.Currentiter.Info;
            Forward (Place);
        end if;

    end Next;

--------------------------------------------------------------------------

    -- generic
--
--         with procedure ModifyItem( Item : in out Itemtype );

    procedure Modifyat (Place : in Listiter) is

    begin
        if (not More (Place)) then
            raise Nomore;
        else
            Modifyitem (Place.Currentiter.Info);
        end if;

    end Modifyat;

--------------------------------------------------------------------------

    -- generic
--
--         with procedure DeleteItem   (
--                   Item     : in out Itemtype;
--                   Delete   : out Boolean );

    procedure Deleteat (L : in out List; Place : in out Listiter) is

        Deleteit : Boolean;
        Temp : List;

    begin
        if (L /= Place.Origlisthead) then
            raise Listitermismatch;
        end if;

        if (not More (Place)) then
            raise Nomore;
        else
            Deleteitem (Place.Currentiter.Info, Deleteit);
            if (Deleteit) then
                Temp := Place.Currentiter;
                Place.Currentiter := Place.Currentiter.Next;
                if (Place.Previousiter = null) then
                    L := Place.Currentiter;  -- new head of list
                    Place.Origlisthead := L; -- reassign list head
                else
                    Place.Previousiter.Next := Place.Currentiter;
                end if;
                Free (Temp);
            end if;
        end if;

    end Deleteat;

--------------------------------------------------------------------------

    procedure Replacehead (L : in out List; Info : in Itemtype) is

        --| This procedure replaces the information at the head of a list
        --| with the given information. If the list is empty the exception
        --| EmptyList is raised.

    begin
        if L = null then
            raise Emptylist;
        else
            L.Info := Info;
        end if;
    end Replacehead;

--------------------------------------------------------------------------

    procedure Replacetail (L : in out List; Newtail : in List) is

        --| This destroys the tail of a list and replaces the tail with
        --| NewTail.  If L is empty EmptyList is raised.

    begin
        Destroy (L.Next);
        L.Next := Newtail;
    exception
        when Constraint_Error =>
            raise Emptylist;
    end Replacetail;

--------------------------------------------------------------------------

    function Tail (L : in List) return List is

        --| This returns the list which is the tail of L.  If L is null
        --| EmptyList is raised.

    begin
        if L = null then
            raise Emptylist;
        else
            return L.Next;
        end if;
    end Tail;

--------------------------------------------------------------------------

    function Cellvalue (I : in Listiter) return Itemtype is

    begin
        if (More (I)) then
            return I.Currentiter.Info;
        else
            raise Nomore;
        end if;

    end Cellvalue;

--------------------------------------------------------------------------
    function Equal (List1 : in List; List2 : in List) return Boolean is

        Placeinlist1 : List;
        Placeinlist2 : List;
--      Contents1 : Itemtype;
--      Contents2 : Itemtype;

        --| This function tests to see if two lists are equal.  Two lists
        --| are equal if for all the elements of List1 the corresponding
        --| element of List2 has the same value.  Thus if the 1st elements
        --| are equal and the second elements are equal and so up to n.
        --|  Thus a necessary condition for two lists to be equal is that
        --| they have the same number of elements.

        --| This function walks over the two list and checks that the
        --| corresponding elements are equal.  As soon as we reach
        --| the end of a list (PlaceInList = null) we fall out of the loop.
        --| If both PlaceInList1 and PlaceInList2 are null after exiting the loop
        --| then the lists are equal.  If they both are not null the lists aren't
        --| equal.  Note that equality on elements is based on a user supplied
        --| function Equal which is used to test for item equality.

    begin
        Placeinlist1 := List1;
        Placeinlist2 := List2;
        while (Placeinlist1 /= null) and (Placeinlist2 /= null) loop
            if not Equal (Placeinlist1.Info, Placeinlist2.Info) then
                return False;
            end if;
            Placeinlist1 := Placeinlist1.Next;
            Placeinlist2 := Placeinlist2.Next;
        end loop;
        return ((Placeinlist1 = null) and (Placeinlist2 = null));
    end Equal;
end Lists;
