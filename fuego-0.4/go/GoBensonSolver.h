//----------------------------------------------------------------------------
/** @file GoBensonSolver.h
    Recognize safe stones and territories according to [Benson 1976].
    
    D.B. Benson. Life in the game of Go. Information Sciences, 10:17Ð29, 1976.
    Reprinted in Computer Games, Levy, D.N.L. (Editor), Vol. II, pp. 203-213,
    Springer Verlag, 1988.
*/
//----------------------------------------------------------------------------

#ifndef GO_BENSONSOLVER_H
#define GO_BENSONSOLVER_H

#include "GoBoard.h"
#include "GoRegion.h"
#include "GoStaticSafetySolver.h"

//----------------------------------------------------------------------------

/** Benson's classic algorithm for finding unconditionally alive blocks */
class GoBensonSolver : public GoStaticSafetySolver
{
public:
    /** If regions = 0, creates its own */
    explicit GoBensonSolver(GoBoard& board, GoRegionBoard* regions = 0)
        : GoStaticSafetySolver(board, regions)
    {
    }

    /** Main function, compute safe points */
    void FindSafePoints(SgBWSet* safe);
};

//----------------------------------------------------------------------------

#endif // GO_BENSONSOLVER_H
