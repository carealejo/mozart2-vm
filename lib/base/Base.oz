functor

require
   Boot_Value     at 'x-oz://boot/Value'
   Boot_Literal   at 'x-oz://boot/Literal'
   Boot_Cell      at 'x-oz://boot/Cell'
   Boot_Atom      at 'x-oz://boot/Atom'
   Boot_Name      at 'x-oz://boot/Name'
   Boot_Int       at 'x-oz://boot/Int'
   Boot_Float     at 'x-oz://boot/Float'
   Boot_Number    at 'x-oz://boot/Number'
   Boot_Tuple     at 'x-oz://boot/Tuple'
   Boot_Dictionary at 'x-oz://boot/Dictionary'
   Boot_Record    at 'x-oz://boot/Record'
   Boot_Chunk     at 'x-oz://boot/Chunk'
   Boot_Array     at 'x-oz://boot/Array'
   Boot_Thread    at 'x-oz://boot/Thread'
   Boot_Exception at 'x-oz://boot/Exception'

   Boot_System    at 'x-oz://boot/System'
   Boot_Space     at 'x-oz://boot/Space'

prepare

   %%
   %% Value
   %%
   Wait         = Boot_Value.wait
   WaitOr       = proc {$ X Y} {Boot_Record.waitOr X#Y _} end
   WaitQuiet    = Boot_Value.waitQuiet
   WaitNeeded   = Boot_Value.waitNeeded
   MakeNeeded   = Boot_Value.makeNeeded
   IsFree       = Boot_Value.isFree
   IsKinded     = Boot_Value.isKinded
   IsFuture     = Boot_Value.isFuture
   IsFailed     = Boot_Value.isFailed
   IsDet        = Boot_Value.isDet
   IsNeeded     = Boot_Value.isNeeded
   Max          = fun {$ A B} if A < B then B else A end end
   Min          = fun {$ A B} if A < B then A else B end end
   CondSelect   = Boot_Value.condSelect
   HasFeature   = Boot_Value.hasFeature
   FailedValue  = Boot_Value.failedValue
   ByNeed       = proc {$ P X} thread {WaitNeeded X} {P X} end end
   ByNeedFuture = fun {$ P}
                     !!{ByNeed fun {$}
                                  try {P}
                                  catch E then {FailedValue E}
                                  end
                               end}
                  end

   %%
   %% Literal
   %%
   IsLiteral = Boot_Literal.is

   %%
   %% Unit
   %%
   IsUnit = fun {$ X} X == unit end

   %%
   %% Cell
   %%
   IsCell   = Boot_Cell.is
   NewCell  = Boot_Cell.new
   Exchange = proc {$ C Old New} Old = {Boot_Cell.exchangeFun C New} end
   Assign   = Boot_Cell.assign
   Access   = Boot_Cell.access

   %%
   %% Atom
   %%
   IsAtom       = Boot_Atom.is
   %AtomToString = Boot_Atom.toString

   %%
   %% Name
   %%
   IsName        = Boot_Name.is
   NewName       = Boot_Name.new
   %NewUniqueName = Boot_Name.newUnique % not exported

   %%
   %% Chunk
   %%
   IsChunk  = Boot_Chunk.is
   NewChunk = Boot_Chunk.new

   %%
   %% Lock
   %%
   local
      LockTag = {NewName}
   in
      fun {IsLock X}
         {IsChunk X} andthen {HasFeature X LockTag}
      end

      fun {NewLock}
         CurrentThread = {NewCell unit}
         Token = {NewCell unit}
         proc {Lock P}
            ThisThread = {Boot_Thread.this}
         in
            if ThisThread == @CurrentThread then
               {P}
            else
               NewToken
            in
               try
                  {Wait Token := NewToken}
                  CurrentThread := ThisThread
                  {P}
               finally
                  CurrentThread := unit
                  NewToken = unit
               end
            end
         end
      in
         {NewChunk 'lock'(LockTag:Lock)}
      end

      proc {LockIn Lock P}
         if {IsLock Lock} then
            {Lock.LockTag P}
         else
            raise typeError('lock' Lock) end
         end
      end
   end

   %%
   %% Port
   %%
   local
      PortTag = {NewName}
   in
      fun {IsPort X}
         {IsChunk X} andthen {HasFeature X PortTag}
      end

      fun {NewPort ?S}
         Head
         Tail = {NewCell Head}
         proc {Send X}
            NewTail
         in
            {Exchange Tail X|!!NewTail NewTail}
         end
      in
         S = !!Head
         {NewChunk port(PortTag:Send)}
      end

      proc {Send P X}
         if {IsPort P} then
            {P.PortTag X}
         else
            raise typeError('port' P) end
         end
      end

      proc {SendRecv P X Y}
         {Send P X#Y}
      end
   end

   %%
   %% Bool
   %%
   IsBool = fun {$ X} X == true orelse X == false end
   Not    = fun {$ X} if X then false else true end end
   And    = fun {$ X Y} X andthen Y end
   Or     = fun {$ X Y} X orelse Y end

   %%
   %% Int
   %%
   IsInt = Boot_Int.is

   %%
   %% Float
   %%
   IsFloat = Boot_Float.is

   %%
   %% Number
   %%
   IsNumber = Boot_Number.is

   %%
   %% Tuple
   %%
   IsTuple   = Boot_Tuple.is
   MakeTuple = Boot_Tuple.make

   %%
   %% Dictionary
   %%
   IsDictionary  = Boot_Dictionary.is
   NewDictionary = Boot_Dictionary.new

   %%
   %% Record
   %%
   IsRecord = Boot_Record.is
   Arity    = Boot_Record.arity
   Label    = Boot_Record.label
   Width    = Boot_Record.width

   local
      fun {CountNewFeatures R1 Fs Acc}
         case Fs
         of H|T then
            if {HasFeature R1 H} then
               {CountNewFeatures R1 T Acc}
            else
               {CountNewFeatures R1 T Acc+1}
            end
         [] nil then
            Acc
         end
      end

      proc {FillTuple1 T R2 Fs Offset}
         case Fs
         of H|Ts then
            T.Offset = H
            T.(Offset+1) = R2.H
            {FillTuple1 T R2 Ts Offset+2}
         [] nil then
            skip
         end
      end

      proc {FillTuple2 T R1 R2 Fs Offset}
         case Fs
         of H|Ts then
            if {HasFeature R2 H} then
               {FillTuple2 T R1 R2 Ts Offset}
            else
               T.Offset = H
               T.(Offset+1) = R1.H
               {FillTuple2 T R1 R2 Ts Offset+2}
            end
         [] nil then
            skip
         end
      end

      fun {FoldL Xs P Z}
         case Xs of nil then Z
         [] X|Xr then {FoldL Xr P {P Z X}}
         end
      end
   in
      fun {Adjoin R1 R2}
         Fs1 = {Arity R1}
         Fs2 = {Arity R2}
         NewWidth = {CountNewFeatures R1 Fs2 {Width R1}}
         T = {MakeTuple '#' NewWidth*2}
      in
         {FillTuple1 T R2 Fs2 1}
         {FillTuple2 T R1 R2 Fs1 {Width R2}*2+1}
         {Boot_Record.makeDynamic {Label R2} T}
      end

      fun {AdjoinAt R F X}
         L = {Label R}
      in
         {Adjoin R L(F:X)}
      end

      fun {AdjoinList R Ts}
         {FoldL Ts
          fun {$ R T}
             {AdjoinAt R T.1 T.2}
          end
          R}
      end
   end

   %%
   %% Array
   %%
   NewArray = Boot_Array.new
   IsArray  = Boot_Array.is
   Put      = Boot_Array.put
   Get      = Boot_Array.get

   %%
   %% Thread
   %%
   IsThread = Boot_Thread.is

   %%
   %% Exception
   %%
   Raise = Boot_Exception.'raise'

   %%
   %% System
   %%
   Show = Boot_System.show

#include "Exception.oz"
#include "Value.oz"
#include "Literal.oz"
#include "Unit.oz"
#include "Lock.oz"
#include "Cell.oz"
#include "Port.oz"
%#include "Atom.oz"
#include "Name.oz"
#include "Bool.oz"
%#include "String.oz"
%#include "Char.oz"
#include "Int.oz"
#include "Float.oz"
#include "Number.oz"
#include "Tuple.oz"
#include "List.oz"
%#include "Procedure.oz"
%#include "Loop.oz"
%#include "WeakDictionary.oz"
#include "Dictionary.oz"
#include "Record.oz"
#include "Chunk.oz"
%#include "VirtualString.oz"
#include "Array.oz"
%#include "Object.oz"
%#include "BitArray.oz"
%#include "ForeignPointer.oz"
#include "Thread.oz"

#include "Space.oz"
#include "System.oz"

end