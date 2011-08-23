//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPRIORKNOWLEDGE_H
#define GOUCT_DEFAULTPRIORKNOWLEDGE_H

#include "GoUctPlayoutPolicy.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Base knowledge class. */
class GoUctKnowledge
{
public:
    GoUctKnowledge(const GoBoard& bd);

    virtual ~GoUctKnowledge();

    virtual void ProcessPosition(std::vector<SgMoveInfo>& moves)=0;

protected:
    const GoBoard& m_bd;

    SgArray<SgStatisticsBase<float,std::size_t>,SG_PASS+1> m_values;

    void Add(SgPoint p, float value, std::size_t count);

    void Initialize(SgPoint p, float value, std::size_t count);

    void ClearValues();

    void TransferValues(std::vector<SgMoveInfo>& outmoves) const;
};

extern float (*defaultValueHeuristic)(const GoBoard *board, SgPoint p);

//----------------------------------------------------------------------------

/** Default prior knowledge heuristic.
    Mainly uses GoUctPlayoutPolicy to generate prior knowledge.
*/
class GoUctDefaultPriorKnowledge 
: public GoUctKnowledge
{
public:
    GoUctDefaultPriorKnowledge(const GoBoard& bd,
                               const GoUctPlayoutPolicyParam& param);

    void ProcessPosition(std::vector<SgMoveInfo>& moves);

    bool FindGlobalPatternAndAtariMoves(SgPointSet& pattern,
                                        SgPointSet& atari,
                                        GoPointList& empty) const;
private:

    GoUctPlayoutPolicy<GoBoard> m_policy;

    void AddLocalityBonus(GoPointList& emptyPoints, bool isSmallBoard);

};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
