//----------------------------------------------------------------------------
/** @file GoSortedMoves.h
    Specialization of SgSortedMoves for Go: move = SgMove, value = int.

    Move tables are used to store a small number of best moves. They
    have the usual operations Insert, Delete, etc.
*/
//----------------------------------------------------------------------------
#ifndef GO_SORTEDMOVES_H
#define GO_SORTEDMOVES_H

#include "SgMove.h"
#include "SgSortedMoves.h"

#define GO_SORTED_MOVES_DEFAULT 3
#define GO_SORTED_MOVES_MAX 20

/** Specialization of SgSortedMoves for Go: move = SgMove, value = int 
	@todo make maxNuMoves a template parameter, use instead 
    of GO_SORTED_MOVES_MAX
*/
class GoSortedMoves : 
	public SgSortedMoves<SgMove, int, GO_SORTED_MOVES_MAX>
{
public:
    explicit GoSortedMoves(int maxNuMoves) : 
        SgSortedMoves<SgMove, int, GO_SORTED_MOVES_MAX>(maxNuMoves)
    {
        Clear();
    }
    
    GoSortedMoves() : 
        SgSortedMoves<SgMove, int, 
                      GO_SORTED_MOVES_MAX>(GO_SORTED_MOVES_DEFAULT)
    {
        Clear();
    }
    
    void Clear()
    {
        SgSortedMoves<SgMove, int, GO_SORTED_MOVES_MAX>::Clear();
        SetInitLowerBound(1);
        SetLowerBound(1);
    }
};

#endif // GO_SORTEDMOVES_H

