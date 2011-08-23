/** @page goboardhash Hash Code in Class GoBoard

@section goboardhashbasics Basics

The hash code is built using Zobrist hashing with random numbers for black
and white stones on the board.
GoBoard::GetHashCode returns the hash code; GoBoard::GetHashCodeInclToPlay
returns the hash code including who is to play.
GoBoard::GetHashCodeInclToPlay is always different from GoBoard::GetHashCode,
independent who is to play.
The hash code is always zero for an empty board, independent of the board
size.

@section goboardhashko Ko Moves

If Ko moves are allowed and GoBoard::KoModifiesHash is true, then the
hash code will also include the number of Ko wins (if there were any).

@section goboardhashhistory Capture History

As a heuristic fix to the Graph-History-Interaction (GHI) problem,
the hash code also includes a component, that depends on the order, in
which stones were captured.
Currently this component cannot be enabled separatly, it is automatically
enabled if GoBoard::KoModifiesHash is true, disabled otherwise.
The idea is to eliminate hashing problems caused by the same
position reached at different level in the search, or recapture
is legal in one branch and illegal in another.
It is not a complete solution to the GHI problem.

*/

