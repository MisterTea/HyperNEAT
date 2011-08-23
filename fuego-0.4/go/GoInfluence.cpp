//----------------------------------------------------------------------------
/** @file GoInfluence.cpp
    See GoInfluence.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoInfluence.h"

#include "SgNbIterator.h"

//----------------------------------------------------------------------------
namespace {
//----------------------------------------------------------------------------

/** Spread influence */
void Spread(const GoBoard& bd, SgPoint p, const SgPointSet& stopPts,
            int val, SgPointArray<int>& influence)
{
    influence[p] += val;
    val /= 2;
    if (val > 0)
        for (SgNb4Iterator it(p); it; ++it)
            if (bd.IsValidPoint(*it) && ! stopPts.Contains(*it))
                Spread(bd, *it, stopPts, val, influence);
}

//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------

void GoInfluence::FindInfluence(const GoBoard& board,
                    int nuExpand,
                    int nuShrink,
                    SgBWSet* influence)
{   
    SgBWSet result = SgBWSet(board.All(SG_BLACK), board.All(SG_WHITE));
    SgBWSet next;
    const int size = board.Size();
    for (int i = 1; i <= nuExpand; ++i)
    {
        for (SgBlackWhite c = SG_BLACK; c <= SG_WHITE; ++c)
        {
            SgBlackWhite opp = SgOppBW(c);
            next[c] = result[c].Border(size) - result[opp];
        }
        result[SG_BLACK] |= (next[SG_BLACK] - next[SG_WHITE]);
        result[SG_WHITE] |= (next[SG_WHITE] - next[SG_BLACK]);
    }

    for (int i = 1; i <= nuShrink; ++i)
    {
        result[SG_BLACK] = result[SG_BLACK].Kernel(size);
        result[SG_WHITE] = result[SG_WHITE].Kernel(size);
    }
    
    *influence = result;
}

int GoInfluence::Influence(const GoBoard& board,
                           SgBlackWhite color,
                           int nuExpand,
                           int nuShrink)
{
    SgBWSet result;
    FindInfluence(board, nuExpand, nuShrink, &result);
    return result[color].Size();
}


void GoInfluence::ComputeInfluence(const GoBoard& bd,
                      const SgBWSet& stopPts,
                      SgBWArray<SgPointArray<int> >* influence)
{
    const int MAX_INFLUENCE = 64;
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color = *it;
        ((*influence)[color]).Fill(0);
        for (GoBoard::Iterator it(bd); it; ++it)
        {
            SgPoint p(*it);
            if (bd.IsColor(p, color))
                Spread(bd, p, stopPts[color], MAX_INFLUENCE, 
                       (*influence)[color]);
        }
    }
}


